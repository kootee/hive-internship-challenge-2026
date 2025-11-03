#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "Slope.h"
#include "Constants.h"

Slope::Slope()
	: _terrainVertices(sf::PrimitiveType::LineStrip)
{
	initialise();
}

Slope::~Slope() {}

void Slope::initialise()
{
	_terrainPts.clear();
	generateTerrain();
	updateVertexArray(); // Build the vertex array after generating terrain
}

void Slope::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (_terrainVertices.getVertexCount() > 0)
	{
		target.draw(_terrainVertices, states);
	}
}

void Slope::clearTerrain()
{
	_terrainPts.clear();
	_terrainVertices.clear();
}

void Slope::updateVertexArray()
{
	// Clear and rebuild the vertex array
	_terrainVertices.clear();
	_terrainVertices.resize(_terrainPts.size());
	
	for (size_t i = 0; i < _terrainPts.size(); ++i)
	{
		_terrainVertices[i].position = _terrainPts[i];
		_terrainVertices[i].color = sf::Color::White;
	}
}

void Slope::shiftPointsToLeft()
{
	if (_terrainPts.empty())
		return;
		
	_terrainPts.pop_front();

	_lastPoint.x = calculateTerrainX(slopeVertexCount - 1); 
	_lastPoint.y = calculateTerrainY(slopeVertexCount - 1);
	
	float slope = getSlope(_terrainPts.back(), _lastPoint);
	float intercept = getIntercept(_terrainPts.back(), slope);

	float rightmostYpos = slope * ScreenWidth + intercept; 
	sf::Vector2f newLastPoint = sf::Vector2f(ScreenWidth, rightmostYpos);
	_terrainPts.push_back(newLastPoint);
	
	updateVertexArray();
}

void Slope::update(float dTime)
{
	if (_terrainPts.empty())
		return;
		
	float leftShift = PlayerSpeed * dTime;
	bool needsShift = false;
	
	for (unsigned long i = 0; i < _terrainPts.size(); i++)
	{		
		_terrainPts[i].x -= leftShift;
		
		if (i < _terrainVertices.getVertexCount())
			_terrainVertices[i].position = _terrainPts[i];
		if (i == 0 && _terrainPts[i].x < -50)
			needsShift = true;
	}
	if (needsShift)
		shiftPointsToLeft();
}

float Slope::getSlope(sf::Vector2f p1, sf::Vector2f p2)
{
	float deltaX = p2.x - p1.x;
	if (std::abs(deltaX) < 0.001f)
		return 0.0f;

	return (p2.y - p1.y) / deltaX;
}

float Slope::getIntercept(sf::Vector2f p, float slope)
{
	return p.y - slope * p.x;
}

void Slope::generateTerrain()
{
	for (int i = 0; i < slopeVertexCount; i++)
	{
		float xPos = calculateTerrainX(i);
		float yPos = calculateTerrainY(i);
		_terrainPts.push_back(sf::Vector2f(xPos, yPos));
	}
	_lastPoint = sf::Vector2f(
					calculateTerrainX(slopeVertexCount + 1), 
					calculateTerrainY(slopeVertexCount + 1)
				);
	updateVertexArray();
}

float Slope::calculateTerrainX(unsigned long i)
{
	if (i == 0)
		return 0;
	
	if (i - 1 >= _terrainPts.size())
		return ScreenWidth;
		
	float prevX = _terrainPts[i - 1].x;
	bool prevIsRamp = false;

	if (i >= 2 && i - 2 < _terrainPts.size())
	{
		prevIsRamp = (_terrainPts[i - 2].y < _terrainPts[i - 1].y) 
					? true : false;
	}
	if (prevIsRamp)
		return prevX;
	else
		return (prevX + ScreenWidth / slopeVertexCount);
}

float Slope::calculateTerrainY(unsigned long i)
{
	int yValueRange[2] = {(ScreenHeight / 4) * 2, (ScreenHeight / 4) * 3};
	
	if (i == 0 || i - 1 >= _terrainPts.size())
		return (rand() % (yValueRange[1] - yValueRange[0] + 1) + yValueRange[0]);
	
	float prevY = _terrainPts[i - 1].y;
	bool prevIsRamp = false;
	
	if (i >= 2 && i - 2 < _terrainPts.size())
	{
		prevIsRamp = (_terrainPts[i - 2].y < _terrainPts[i - 1].y) 
					? true : false;
	}
	if (prevIsRamp)
		return prevY - RampHeight;

	return (rand() % (yValueRange[1] - yValueRange[0] + 1) + yValueRange[0]);
}

const std::deque<sf::Vector2f>& Slope::getTerrainPoints() const
{
	return _terrainPts;
}

const sf::VertexArray& Slope::getTerrainVertices() const
{
	return _terrainVertices;
}
