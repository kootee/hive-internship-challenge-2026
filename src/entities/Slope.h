#pragma once

#include <memory>
#include <map>
#include <vector>
#include <deque>
#include <SFML/Graphics.hpp>


class Slope : public sf::Drawable
{
	private:
		std::deque<sf::Vector2f>	_terrainPts;
		sf::Vector2f				_lastPoint;
		sf::VertexArray				_terrainVertices;

	public:
		Slope();
		virtual ~Slope();

		// General functions
		void generateTerrain();
		void clearTerrain();
		void initialise();
		void update(float dTime);
		void shiftPointsToLeft();
		
		// Graphics
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
		void updateVertexArray();
		
		// Slope calculations
		float getSlope(sf::Vector2f p1, sf::Vector2f p2);
		float getIntercept(sf::Vector2f p1, float p2);
		float calculateTerrainX(unsigned long i);
		float calculateTerrainY(unsigned long i);

		// Getters
		const std::deque<sf::Vector2f>& getTerrainPoints() const;
		const sf::VertexArray& 			getTerrainVertices() const;
};