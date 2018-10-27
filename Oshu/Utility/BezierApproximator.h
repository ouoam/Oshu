#pragma once
#include <stack>
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics/VertexArray.hpp>

// Change From https://github.com/ppy/osu/blob/master/osu.Game/Rulesets/Objects/CircularArcApproximator.cs
class BezierApproximator
{
	int count;
	sf::VertexArray controlPoints;
	std::vector<sf::Vector2f> subdivisionBuffer1;
	std::vector<sf::Vector2f> subdivisionBuffer2;

	const float tolerance = 0.25f;
	const float tolerance_sq = tolerance * tolerance;

	// More info https://msdn.microsoft.com/en-us/library/system.windows.vector.lengthsquared(v=vs.110).aspx
	inline float LengthSquared(sf::Vector2f a)
	{
		return a.x * a.x + a.y * a.y;
	}

	// More info https://www.mathsisfun.com/algebra/vectors-dot-product.html
	// More info https://en.wikipedia.org/wiki/Dot_product
	inline float Dot(sf::Vector2f a, sf::Vector2f b)
	{
		return a.x * b.x + a.y * b.y;
	}

	std::vector<sf::Vector2f> toVector(sf::VertexArray a)
	{
		int n = a.getVertexCount();
		std::vector<sf::Vector2f> out;
		for (int i = 0; i < n; i++)
		{
			out.push_back((sf::Vector2f) a[i].position);
		}
		return out;
	}

	/// <summary>
	/// Make sure the 2nd order derivative (approximated using finite elements) is within tolerable bounds.
	/// NOTE: The 2nd order derivative of a 2d curve represents its curvature, so intuitively this function
	///       checks (as the name suggests) whether our approximation is _locally_ "flat". More curvy parts
	///       need to have a denser approximation to be more "flat".
	/// </summary>
	/// <param name="controlPoints">The control points to check for flatness.</param>
	/// <returns>Whether the control points are flat enough.</returns>
	bool isFlatEnough(std::vector<sf::Vector2f> &controlPoints)
	{
		for (int i = 1; i < controlPoints.size() - 1; i++)
			if (LengthSquared(controlPoints[i - 1] - 2.0f * controlPoints[i] + controlPoints[i + 1]) > tolerance_sq * 4)
				return false;

		return true;
	}

	/// <summary>
	/// Subdivides n control points representing a bezier curve into 2 sets of n control points, each
	/// describing a bezier curve equivalent to a half of the original curve. Effectively this splits
	/// the original curve into 2 curves which result in the original curve when pieced back together.
	/// </summary>
	/// <param name="controlPoints">The control points to split.</param>
	/// <param name="l">Output: The control points corresponding to the left half of the curve.</param>
	/// <param name="r">Output: The control points corresponding to the right half of the curve.</param>
	void subdivide(std::vector<sf::Vector2f> &controlPoints, std::vector<sf::Vector2f> &l, std::vector<sf::Vector2f> &r)
	{
		std::vector<sf::Vector2f> &midpoints = subdivisionBuffer1;

		for (int i = 0; i < count; ++i)
			midpoints[i] = controlPoints[i];

		for (int i = 0; i < count; i++)
		{
			l[i] = midpoints[0];
			r[count - i - 1] = midpoints[count - i - 1];

			for (int j = 0; j < count - i - 1; j++)
				midpoints[j] = (midpoints[j] + midpoints[j + 1]) / 2.0f;
		}
	}

	/// <summary>
	/// This uses <a href="https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm">De Casteljau's algorithm</a> to obtain an optimal
	/// piecewise-linear approximation of the bezier curve with the same amount of points as there are control points.
	/// </summary>
	/// <param name="controlPoints">The control points describing the bezier curve to be approximated.</param>
	/// <param name="output">The points representing the resulting piecewise-linear approximation.</param>
	void approximate(std::vector<sf::Vector2f> &controlPoints, sf::VertexArray &output)
	{
		std::vector<sf::Vector2f> &l = subdivisionBuffer2;
		std::vector<sf::Vector2f> &r = subdivisionBuffer1;

		subdivide(controlPoints, l, r);

		for (int i = 0; i < count - 1; ++i)
			l[count + i] = r[i + 1];

		output.append(controlPoints[0]);
		for (int i = 1; i < count - 1; ++i)
		{
			int index = 2 * i;
			sf::Vector2f p = 0.25f * (l[index - 1] + 2.0f * l[index] + l[index + 1]);
			output.append(p);
		}
	}

public:
	BezierApproximator(sf::VertexArray controlPoints)
	{
		this->controlPoints = controlPoints;
		this->count = controlPoints.getVertexCount();

		subdivisionBuffer1.resize(count);
		subdivisionBuffer2.resize(count * 2 - 1);
	}

	sf::VertexArray CreateBezier()
	{
		sf::VertexArray output(sf::LineStrip);

		if (count == 0)
			return output;

		std::stack<std::vector<sf::Vector2f>> toFlatten;
		std::stack<std::vector<sf::Vector2f>> freeBuffers;

		// "toFlatten" contains all the curves which are not yet approximated well enough.
		// We use a stack to emulate recursion without the risk of running into a stack overflow.
		// (More specifically, we iteratively and adaptively refine our curve with a
		// <a href="https://en.wikipedia.org/wiki/Depth-first_search">Depth-first search</a>
		// over the tree resulting from the subdivisions we make.)
		toFlatten.push(toVector(controlPoints));

		std::vector<sf::Vector2f> &leftChild = subdivisionBuffer2;

		while (toFlatten.size() > 0)
		{
			std::vector<sf::Vector2f> parent = toFlatten.top();
			toFlatten.pop();
			if (isFlatEnough(parent))
			{
				// If the control points we currently operate on are sufficiently "flat", we use
				// an extension to De Casteljau's algorithm to obtain a piecewise-linear approximation
				// of the bezier curve represented by our control points, consisting of the same amount
				// of points as there are control points.
				approximate(parent, output);
				freeBuffers.push(parent);
				continue;
			}

			// If we do not yet have a sufficiently "flat" (in other words, detailed) approximation we keep
			// subdividing the curve we are currently operating on.
			std::vector<sf::Vector2f> rightChild;
			if (freeBuffers.size() > 0) {
				rightChild = freeBuffers.top();
				freeBuffers.pop();
			}
			else
			{
				rightChild = *new std::vector<sf::Vector2f>;
				rightChild.resize(count);
			}
			subdivide(parent, leftChild, rightChild);

			// We re-use the buffer of the parent for one of the children, so that we save one allocation per iteration.
			for (int i = 0; i < count; ++i)
				parent[i] = leftChild[i];

			toFlatten.push(rightChild);
			toFlatten.push(parent);
		}

		output.append(controlPoints[count - 1]);
		return output;
	}
};