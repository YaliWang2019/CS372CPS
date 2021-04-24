//
// Created by Chris Hartman on 3/5/21.
//
// Modified by Yali Wang on 4/23/21.

#ifndef CS372CPS_SHAPE_HPP
#define CS372CPS_SHAPE_HPP

#include <memory>
#include <string>
#include <vector>

class Shape;
using ShapePtr = std::shared_ptr<Shape>;

class Shape {
private:
public:
   [[nodiscard]] virtual double getHeight() const = 0;
   [[nodiscard]] virtual double getWidth() const = 0;
   [[nodiscard]] virtual std::string getPostScript() const = 0;
};

class Circle : public Shape {
private:
   double _radius;

public:
   explicit Circle(double radius);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
};

class Rectangle : public Shape {
private:
   double _width;
   double _height;

public:
   explicit Rectangle(double width, double height);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
};

class Spacer : public Shape {
private:
   double _width;
   double _height;

public:
   explicit Spacer(double width, double height);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
};

class Polygon : public Shape {
private:
   int _numSides;
   double _sideLength;

public:
   Polygon(int numSides, double sideLength);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
};

class Triangle : public Polygon {
public:
   explicit Triangle(double sideLength);
};

class Square : public Polygon {
public:
   explicit Square(double sideLength);
};

class ScaledShape : public Shape {
public:
   explicit ScaledShape(ShapePtr shape, double xScale, double yScale);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
private:
   ShapePtr _shape;
   double _xScale;
   double _yScale;
};

enum class Rot {R90,R180,R270};

class RotatedShape : public Shape {
public:
   explicit RotatedShape(ShapePtr shape, Rot rotation);
   [[nodiscard]] double getHeight() const override;
   [[nodiscard]] double getWidth() const override;
   [[nodiscard]] std::string getPostScript() const override;
private:
   ShapePtr _shape;
   Rot _rotation;
};

// Add CompoundShapes Class
class CompoundShapes : public Shape {

private:
   std::vector<std::shared_ptr<Shape>> _shapeList;
   double _width = 0;
   double _height = 0;

protected:
   [[nodiscard]] std::vector<std::shared_ptr<Shape>> getShapes() const {
      return _shapeList;
   }
   [[nodiscard]] virtual std::string moveToPositionForShape() const = 0;
   [[nodiscard]] virtual std::string moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const = 0;

public:
   CompoundShapes(std::initializer_list<std::shared_ptr<Shape>> list) : _shapeList(list) {
   }
   virtual ~CompoundShapes() = default;

   [[nodiscard]] std::string getPostScript() const override;
};


class LayeredShape : public CompoundShapes {
private:
   double _width;
   double _height;

   [[nodiscard]] std::string moveToPositionForShape() const override;
   [[nodiscard]] std::string moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const override;

public:
   [[nodiscard]] double getWidth() const override {
      return _width;
   }
   [[nodiscard]] double getHeight() const override {
      return _height;
   }
   LayeredShape(std::initializer_list<std::shared_ptr<Shape>> list);
};

class VerticalShape : public CompoundShapes {
private:
   double _width;
   double _height;

   [[nodiscard]] std::string moveToPositionForShape() const override;
   [[nodiscard]] std::string moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const override;

public:
   [[nodiscard]] double getWidth() const override {
      return _width;
   }
   [[nodiscard]] double getHeight() const override {
      return _height;
   }
   VerticalShape(std::initializer_list<std::shared_ptr<Shape>> list);
};

class HorizontalShape : public CompoundShapes {
private:
   double _width;
   double _height;

   [[nodiscard]] std::string moveToPositionForShape() const override;
   [[nodiscard]] std::string moveHalfwayForShape(const std::shared_ptr<Shape> &shape) const override;

public:
   [[nodiscard]] double getWidth() const override {
      return _width;
   }
   [[nodiscard]] double getHeight() const override {
      return _height;
   }
   HorizontalShape(std::initializer_list<std::shared_ptr<Shape>> list);
};

ShapePtr makeCircle(double radius);
ShapePtr makeRectangle(double width, double height);
ShapePtr makeSpacer(double width, double height);
ShapePtr makePolygon(int numSides, double sideLength);
ShapePtr makeTriangle(double sideLength);
ShapePtr makeSquare(double sideLength);
ShapePtr makeScaledShape(ShapePtr shape, double xScale, double yScale);
ShapePtr makeRotatedShape(ShapePtr shape, Rot rotation);
ShapePtr makeLayeredShape(std::initializer_list<ShapePtr> shapes);
ShapePtr makeVerticalShape(std::initializer_list<ShapePtr> shapes);
ShapePtr makeHorizontalShape(std::initializer_list<ShapePtr> shapes);
#endif//CS372CPS_SHAPE_HPP
