#pragma once
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

#include "../Library/clipper_ver6.4.2/cpp/clipper.hpp"

namespace sf
{
	// edit     from https://github.com/SFML/SFML/wiki/Source:-line-with-thickness
	// add code from https://github.com/SFML/SFML/wiki/Source:-Line-segment-with-thickness

	//float GetMagnitude(Vector2f v)
	//{
	//	return (float)sqrt(v.x * v.x + v.y * v.y);
	//}

	//float Dot(Vector2f v0, Vector2f v1)
	//{
	//	return v0.x * v1.x + v0.y * v1.y;
	//}

	//Vector2f GetNormal(Vector2f v)
	//{
	//	return Vector2f(-v.y, v.x);
	//}

	//Vector2f Normalized(Vector2f v)
	//{
	//	return v / GetMagnitude(v);
	//}

	//VertexArray GenerateTrianglesStrip(sf::VertexArray points, float thickness)
	//{
	//	sf::VertexArray output(sf::TrianglesStrip);
	//	int count = points.getVertexCount();
	//	for (int i = 0; i < count; i++)
	//	{
	//		Vector2f v1 = points[i].position;
	//		Vector2f offset;

	//		if (i == 0 || i == count - 1) {
	//			Vector2f vA = points[(i == 0) ? 1 : count - 2].position;
	//			offset = GetNormal(Normalized((i == 0) ? (vA - v1) : (v1 - vA)));
	//		}
	//		else
	//		{
	//			Vector2f v0 = points[i - 1].position;
	//			Vector2f v2 = points[i + 1].position;
	//			Vector2f v01 = Normalized(v1 - v0);
	//			Vector2f v12 = Normalized(v2 - v1);
	//			offset = GetNormal(v01 + v12);
	//			float dot = Dot(offset, GetNormal(v01));
	//			//float dot = 1.f + Dot(v01, v12); // From https://github.com/SFML/SFML/blob/ac98be760b8b1401fd7a3b141883cbf04feba7b7/src/SFML/Graphics/Shape.cpp#L286

	//			offset /= dot; //< TODO: Add flat miter joint in extreme cases
	//		}
	//		offset *= thickness / 2.0f;
	//		output.append(Vertex(v1 + offset));
	//		output.append(Vertex(v1 - offset));
	//	}
	//	
	//	return output;
	//}
	//


	VertexArray GenerateTrianglesStrip(sf::VertexArray points, float thickness) {
		sf::VertexArray output(sf::LineStrip);
		ClipperLib::Path in;
		ClipperLib::Paths sol;

		int count = points.getVertexCount();

		for (int i = 0; i < count; i++) {
			Vector2f v1 = points[i].position;
			in.push_back(ClipperLib::IntPoint(v1.x * 10.0, v1.y * 10.0));
		}

		ClipperLib::ClipperOffset co;
		co.AddPath(in, ClipperLib::jtRound, ClipperLib::etOpenRound);
		co.Execute(sol, thickness / 2.0 * 10.0);


		for (ClipperLib::Path path : sol) {
			for (ClipperLib::IntPoint point : path) {
				output.append(sf::Vertex(sf::Vector2f(point.X / 10.0, point.Y / 10.0)));
			}
			output.append(sf::Vertex(sf::Vector2f(path[0].X / 10.0, path[0].Y / 10.0)));
		}

		return output;
	}
}
