#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "Slope.h"
#include "Constants.h"

Slope::Slope()
	: _terrainVertices(sf::PrimitiveType::LineStrip, 2)
{
	initialise();
}

Slope::~Slope() {}

void Slope::initialise()
{
	_terrainPts.clear();
	generateTerrain();
	updateVertexArray();
}

void Slope::generateTerrain()
{
	sf::Vector2f slopeUpperBound = sf::Vector2f(0, 100);
	sf::Vector2f slopeLowerBound = sf::Vector2f(1000, 0);

	_terrainPts.push_back(slopeUpperBound);
	_terrainPts.push_back(slopeLowerBound);
	
	updateVertexArray();
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
	if (_terrainPts.empty())
	{
		_terrainVertices.clear();
		return;
	}

	_terrainVertices.clear();
	_terrainVertices.resize(_terrainPts.size());

	_terrainVertices[0].position = sf::Vector2f(50.0f, 50.0f);
	_terrainVertices[1].position = sf::Vector2f(50.0f, 50.0f);
	

	for (size_t i = 0; i < _terrainPts.size(); ++i)
	{
		_terrainVertices[i].position = _terrainPts[i];
		_terrainVertices[i].color = sf::Color::Red;
	}

	_terrainVertices[_terrainPts.size() - 1].position = sf::Vector2f((1024.0f - 50.f), 0);
	_terrainVertices[_terrainPts.size() - 1].color = sf::Color::Red;
}

void Slope::shiftPointsToLeft()
{
	if (_terrainPts.empty())
		return;
		
	_terrainPts.pop_front();

	float newX = calculateTerrainX(_terrainPts.size() - 1); 
	float newY = calculateTerrainY(_terrainPts.size() - 1);
	
	sf::Vector2f newLastPoint = sf::Vector2f(newX, newY);
	_terrainPts.push_back(newLastPoint);
	
	updateVertexArray();
}

void Slope::update(float dTime)
{
if (_terrainPts.empty())
        return;

    float dx = GameSpeed * dTime;
    float dy = GameSpeed * dTime;

    bool needsShift = false;

    for (unsigned long i = 0; i < _terrainPts.size(); ++i)
    {
        _terrainPts[i].x -= dx;
        _terrainPts[i].y += dy;

        if (i < _terrainVertices.getVertexCount())
        {
            _terrainVertices[i].position.x = _terrainPts[i].x;
            _terrainVertices[i].position.y = _terrainPts[i].y;
        }

        if (i == 0 && _terrainPts[i].x < -50.0f)
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

float Slope::calculateTerrainX(unsigned long i)
{
	if (i == 0)
		return 0;
	
	if (i - 1 >= _terrainPts.size())
		return ScreenWidth;
		
	float prevX = _terrainPts[i - 1].x;
	return (prevX + ScreenWidth / slopeVertexCount);
}

float Slope::calculateTerrainY(unsigned long i)
{	
	// First vertex
	if (i == 0)
	{
		return (rand() % 
		((ScreenHeight / 4) * 3 - (ScreenHeight / 4) * 2 + 1) + 
		(ScreenHeight / 4) * 3);
	}
	
	float prevY = _terrainPts[i - 1].y;
	int yRange[2];
	
	int makeRamp = (rand() % 3 == 0) ? true : false;
	
	if (makeRamp)
	{
		yRange[0] = prevY - 150;
		yRange[1] = prevY;
	}
	else
	{
		yRange[0] = prevY;
		yRange[1] = 896;
	}
	// Check for ramp
	/* 	if (i >= 2 && i - 2 < _terrainPts.size())
	{
		prevIsRamp = (_terrainPts[i - 2].y < _terrainPts[i - 1].y) 
					? true : false;
	}
	if (prevIsRamp)
		return prevY - RampHeight; */

	return (rand() % (yRange[1] - yRange[0] + 1) + yRange[0]);
}

const std::deque<sf::Vector2f>& Slope::getTerrainPoints() const
{
	return _terrainPts;
}

const sf::VertexArray& Slope::getTerrainVertices() const
{
	return _terrainVertices;
}
