//
// Created by Chris Hartman on 3/5/21.
//
// Modified by Yali Wang on 4/23/21.

#include "Shape.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>

const double pi = 3.14159265358979323846264;

Circle::Circle(double radius) : _radius(radius) {
}

double Circle::getHeight() const {
   return 2 * _radius;
}

double Circle::getWidth() const {
   return 2 * _radius;
}

std::string Circle::getPostScript() const {
   return "gsave currentpoint translate newpath 0 0 " + std::to_string(_radius) + " 0 360 arc closepath stroke grestore\n";
}

Rectangle::Rectangle(double width, double height) : _width(width), _height(height) {
}

double Rectangle::getHeight() const {
   return _height;
}

double Rectangle::getWidth() const {
   return _width;
}

std::string Rectangle::getPostScript() const {
   // when writing output to PostScript we start at the
   // center of the shape's bounding box, so begin the
   // drawing by moving from the center to the first vertex (bottom left)
   // then proceed to draw lines "relative to that position"
   // connecting to the remaining vertices

   std::string output{"gsave\n"};

   output += std::to_string(-getWidth() / 2) + " ";
   output += std::to_string(-getHeight() / 2) + " rmoveto\n";
   output += std::to_string(getWidth()) + " 0 rlineto\n";
   output += " 0 " + std::to_string(getHeight()) + " rlineto\n";
   output += "-" + std::to_string(getWidth()) + " 0 rlineto\n";
   output += "closepath\nstroke\ngrestore\n";

   return output;
}

Spacer::Spacer(double width, double height) : _width(width), _height(height) {
}

double Spacer::getHeight() const {
   return _height;
}

double Spacer::getWidth() const {
   return _width;
}

std::string Spacer::getPostScript() const {
   return {};
}

Polygon::Polygon(int numSides, double sideLength) : _numSides(numSides), _sideLength(sideLength) {
}

double Polygon::getHeight() const {
   if (_numSides % 2 == 0)
      return _sideLength * cos(pi / _numSides) / sin(pi / _numSides);
   return _sideLength * (1 + cos(pi / _numSides)) / (2 * sin(pi / _numSides));
}

double Polygon::getWidth() const {
   if (_numSides % 4 == 0)
      return _sideLength * cos(pi / _numSides) / sin(pi / _numSides);
   if (_numSides % 2 == 0)
      return _sideLength / sin(pi / _numSides);
   return _sideLength * sin(pi * (_numSides - 1) / (2 * _numSides)) / sin(pi / _numSides);
}

std::string Polygon::getPostScript() const {
   std::string output{"gsave\n"};

   // start by moving to bottom left vertex of PolygonShape (first side drawn is always horizontal at bottom)
   output += std::to_string(-_sideLength / 2) + " ";
   output += std::to_string(-getHeight() / 2);
   output += " rmoveto\n";

   for (int i = 0; i < _numSides - 1; i++) {
      double current_direction = i * 2 * pi / _numSides;
      double next_x = _sideLength * cos(current_direction);
      double next_y = _sideLength * sin(current_direction);
      output += std::to_string(next_x) + " " + std::to_string(next_y) + " rlineto\n";
   }
   output += "closepath\nstroke\ngrestore\n";
   return output;
}

Square::Square(double sideLength) : Polygon(4, sideLength) {
}

Triangle::Triangle(double sideLength) : Polygon(3, sideLength) {
}

ScaledShape::ScaledShape(ShapePtr shape, double xScale, double yScale)
    : _shape(std::move(shape)), _xScale(xScale), _yScale(yScale) {
}

double ScaledShape::getHeight() const {
   return _yScale * _shape->getHeight();
}

double ScaledShape::getWidth() const {
   return _xScale * _shape->getWidth();
}

std::string ScaledShape::getPostScript() const {
   return std::string();
}

RotatedShape::RotatedShape(ShapePtr shape, Rot rotation) : _shape(std::move(shape)), _rotation(rotation) {
}

double RotatedShape::getHeight() const {
   if (_rotation == Rot::R180)
      return _shape->getHeight();
   return _shape->getWidth();
}

double RotatedShape::getWidth() const {
   if (_rotation == Rot::R180)
      return _shape->getWidth();
   return _shape->getHeight();
}

std::string RotatedShape::getPostScript() const {
   std::string output{"gsave\n"};
   if(_rotation == Rot::R90)
      output += "90";
   else if (_rotation == Rot::R180)
      output += "180";
   else if (_rotation == Rot::R270)
      output += "270";
   output += " rotate\n";
   output += _shape->getPostScript();
   output += "grestore\n";

   return output;
}


std::string CompoundShapes::getPostScript() const {
   std::string output{"gsave\n"};

   output += moveToPositionForShape();
   for (const auto &shape : getShapes()) {
      output += moveHalfwayForShape(shape);
      output += shape->getPostScript();
      output += moveHalfwayForShape(shape);
      output += "\n";
   }

   output += "grestore\n";

   return output;
}

LayeredShape::LayeredShape(std::initializer_list<std::shared_ptr<Shape>> list) : CompoundShapes(list) {
   double width = 0;
   double height = 0;
   for (auto const &shape : list) {
      if (shape->getHeight() > height)
         height = shape->getHeight();
      if (shape->getWidth() > width)
         width = shape->getWidth();
   }
   _width = width;
   _height = height;
}

std::string LayeredShape::moveToPositionForShape() const {
   return std::string{};
}

std::string LayeredShape::moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const {
   return std::string{};
}

std::shared_ptr<LayeredShape> Layered(std::initializer_list<std::shared_ptr<Shape>> list) {
   return std::make_shared<LayeredShape>(list);
}

VerticalShape::VerticalShape(std::initializer_list<std::shared_ptr<Shape>> list) : CompoundShapes(list){
   double width = 0;
   double height = 0;
   for (auto const &shape : list) {
      height += shape->getHeight();
      if (shape->getWidth() > width)
         width = shape->getWidth();
   }
   _width = width;
   _height = height;
}

std::string VerticalShape::moveToPositionForShape() const {
   return "0 " + std::to_string(- getHeight() / 2) + " rmoveto\n";
}

std::string VerticalShape::moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const {
   return "0 " + std::to_string(shape-> getHeight() / 2) + " rmoveto\n";
}

std::shared_ptr<VerticalShape> Vertical(std::initializer_list<std::shared_ptr<Shape>> list) {
   return std::make_shared<VerticalShape>(list);
}

HorizontalShape::HorizontalShape(std::initializer_list<std::shared_ptr<Shape>> list) : CompoundShapes(list){
   double width = 0;
   double height = 0;
   for (auto const &shape : list) {
      width += shape->getWidth();
      if (shape->getHeight() > height)
         height = shape->getHeight();
   }
   _width = width;
   _height = height;
}

std::string HorizontalShape::moveToPositionForShape() const {
   return std::to_string(- getWidth() / 2) + " 0 rmoveto\n";
}

std::string HorizontalShape::moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const {
   return std::to_string(shape-> getWidth() / 2) + " 0 rmoveto\n";
}

std::shared_ptr<HorizontalShape> Horizontal(std::initializer_list<std::shared_ptr<Shape>> list) {
   return std::make_shared<HorizontalShape>(list);
}

ShapePtr makeCircle(double radius) {
   return std::make_shared<Circle>(radius);
}

ShapePtr makeRectangle(double width, double height) {
   return std::make_shared<Rectangle>(width, height);
}

ShapePtr makeSpacer(double width, double height) {
   return std::make_shared<Spacer>(width, height);
}

ShapePtr makePolygon(int numSides, double sideLength) {
   return std::make_shared<Polygon>(numSides, sideLength);
}

ShapePtr makeSquare(double sideLength) {
   return std::make_shared<Square>(sideLength);
}

ShapePtr makeTriangle(double sideLength) {
   return std::make_shared<Triangle>(sideLength);
}

ShapePtr makeScaledShape(ShapePtr shape, double xScale, double yScale) {
   return std::make_shared<ScaledShape>(shape, xScale, yScale);
}

ShapePtr makeRotatedShape(ShapePtr shape, Rot rotation) {
   return std::make_shared<RotatedShape>(shape, rotation);
}

ShapePtr makeLayeredShape(std::initializer_list<ShapePtr> shapes) {
   return std::make_shared<LayeredShape>(shapes);
}

ShapePtr makeVerticalShape(std::initializer_list<ShapePtr> shapes) {
   return std::make_shared<VerticalShape>(shapes);
}

ShapePtr makeHorizontalShape(std::initializer_list<ShapePtr> shapes) {
   return std::make_shared<HorizontalShape>(shapes);
}
