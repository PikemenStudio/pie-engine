//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_TUNNEL_H
#define ENGINE_TUNNEL_H

#include <vector>

#include <SFML/Graphics.hpp>

#include "Drawable.h"
#include "SolidObject.h"

class WorldWindow;
class Player;

class Tunnel : public SolidObject, public Drawable
{
public:
  Tunnel(float StartX, float StepX, int PointsCount, float PlWidth, float PlHeight,
         bool ClosedLeft, bool ClosedRight);
  Tunnel(float StartX, float EndX, float StepX, float PlWidth, float PlHeight,
         bool ClosedLeft, bool ClosedRight);

  bool isCollision(const SolidObject* Other) const override;
  bool isCollisionWithPlayer(const Player* Pl) const;

  void setVisible(bool V) { Visible = V; }
  bool isVisible() const { return Visible; }

  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) override;

  float getStartX() const { return StartX; }
  float getEndX() const { return StartX + (WorldCoordsYFloor.size() - 1) * StepX; }

  float getFloorYCoord(float XCoord) const;

//  float getStepX() const { return StepX; }
//  const std::vector<float>& getFloorCoords() const { return WorldCoordsYFloor; }
//  const std::vector<float>& getCeilingCoords() const { return WorldCoordsYCeiling; }

private:
  void drawTunnelPart(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj,
                      const std::vector<float>& WorldCoordsY, bool IsFloor) const;

  float StartX = 0.0;
  float StepX = 0.1;
  std::vector<float> WorldCoordsYFloor;
  std::vector<float> WorldCoordsYCeiling;

  bool Visible = true;
};

#endif // ENGINE_TUNNEL_H
