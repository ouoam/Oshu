#pragma once
#include <cmath>
#include <SFML/System.hpp>
#include <SFML/Graphics/VertexArray.hpp>

// Change From https://github.com/ppy/osu/blob/master/osu.Game/Rulesets/Objects/CircularArcApproximator.cs
class CircularArcApproximator
{
	sf::Vector2f a;
	sf::Vector2f b;
	sf::Vector2f c;

	int amountPoints;

	const float tolerance = 0.1f;

	// More info https://msdn.microsoft.com/en-us/library/system.windows.vector.lengthsquared(v=vs.110).aspx
	inline float LengthSquared(sf::Vector2f a)
	{
		return a.x * a.x + a.y * a.y;
	}

	inline float Length(sf::Vector2f a)
	{
		return sqrt(a.x * a.x + a.y * a.y);
	}

	// More info https://www.mathsisfun.com/algebra/vectors-dot-product.html
	inline float Dot(sf::Vector2f a, sf::Vector2f b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline int max(int a, int b)
	{
		return a > b ? a : b;
	}

	const double M_PI = 3.14159265358979323846;

public:

	CircularArcApproximator(sf::VertexArray a)
	{
		this->a = a[0].position;
		this->b = a[1].position;
		this->c = a[2].position;
	}

	/// <summary>
	/// Creates a piecewise-linear approximation of a circular arc curve.
	/// </summary>
	/// <returns>A list of vectors representing the piecewise-linear approximation.</returns>
	sf::VertexArray CreateArc()
	{
		float aSq = LengthSquared(b - c);
		float bSq = LengthSquared(a - c);
		float cSq = LengthSquared(a - b);


		// If we have a degenerate triangle where a side-length is almost zero, then give up and fall
		// back to a more numerically stable method.
		if (aSq == 0 || bSq == 0 || cSq == 0)
			return *new sf::VertexArray;


		float s = aSq * (bSq + cSq - aSq);
		float t = bSq * (aSq + cSq - bSq);
		float u = cSq * (aSq + bSq - cSq);


		float sum = s + t + u;


		// If we have a degenerate triangle with an almost-zero size, then give up and fall
		// back to a more numerically stable method.
		if (sum == 0)
			return *new sf::VertexArray;

		sf::Vector2f centre = (s * a + t * b + u * c) / sum;
		sf::Vector2f dA = a - centre;
		sf::Vector2f dC = c - centre;

		float r = Length(dA);

		double thetaStart = atan2(dA.y, dA.x);
		double thetaEnd = atan2(dC.y, dC.x);

		while (thetaEnd < thetaStart)
			thetaEnd += 2 * M_PI;

		double dir = 1;
		double thetaRange = thetaEnd - thetaStart;

		// Decide in which direction to draw the circle, depending on which side of
		// AC B lies.
		sf::Vector2f orthoAtoC = c - a;
		orthoAtoC = *new sf::Vector2f(orthoAtoC.y, -orthoAtoC.x);
		if (Dot(orthoAtoC, b - a) < 0)
		{
			dir = -dir;
			thetaRange = 2 * M_PI - thetaRange;
		}

		// We select the amount of points for the approximation by requiring the discrete curvature
		// to be smaller than the provided tolerance. The exact angle required to meet the tolerance
		// is: 2 * Math.Acos(1 - TOLERANCE / r)
		// The special case is required for extremely short sliders where the radius is smaller than
		// the tolerance. This is a pathological rather than a realistic case.
		amountPoints = 2 * r <= tolerance ? 2 : max(2, (int)ceil(thetaRange / (2 * acos(1 - tolerance / r))));

		sf::VertexArray output(sf::LineStrip);

		for (int i = 0; i < amountPoints; ++i)
		{
			double fract = (double)i / (amountPoints - 1);
			double theta = thetaStart + dir * fract * thetaRange;
			sf::Vector2f o((float)cos(theta), (float)sin(theta));
			output.append(sf::Vertex(centre + (o * r)));
		}

		return output;
	}
};