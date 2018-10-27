#pragma once
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
namespace sf
{
	// edit     from https://github.com/SFML/SFML/wiki/Source:-line-with-thickness
	// add code from https://github.com/SFML/SFML/wiki/Source:-Line-segment-with-thickness

	float GetMagnitude(Vector2f v)
	{
		return (float)sqrt(v.x * v.x + v.y * v.y);
	}

	float Dot(Vector2f v0, Vector2f v1)
	{
		return v0.x * v1.x + v0.y * v1.y;
	}

	Vector2f GetNormal(Vector2f v)
	{
		return Vector2f(-v.y, v.x);
	}

	Vector2f Normalized(Vector2f v)
	{
		return v / GetMagnitude(v);
	}

	VertexArray GenerateTrianglesStrip(sf::VertexArray points, float thickness)
	{
		sf::VertexArray output(sf::TrianglesStrip);
		int count = points.getVertexCount();
		for (int i = 0; i < count; i++)
		{
			Vector2f v1 = points[i].position;
			Vector2f offset;

			if (i == 0 || i == count - 1) {
				Vector2f vA = points[(i == 0) ? 1 : count - 2].position;
				offset = GetNormal(Normalized((i == 0) ? (vA - v1) : (v1 - vA)));
			}
			else
			{
				Vector2f v0 = points[i - 1].position;
				Vector2f v2 = points[i + 1].position;
				Vector2f v01 = Normalized(v1 - v0);
				Vector2f v12 = Normalized(v2 - v1);
				offset = GetNormal(v01 + v12);
				float dot = Dot(offset, GetNormal(v01));
				offset /= dot; //< TODO: Add flat miter joint in extreme cases
			}
			offset *= thickness / 2.0f;
			output.append(Vertex(v1 + offset));
			output.append(Vertex(v1 - offset));
		}
		
		return output;
	}
}
