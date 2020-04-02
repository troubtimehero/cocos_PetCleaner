#include "SpritePolygon.h"

SpritePolygon* SpritePolygon::MyCreate(const char* filename, PhysicsMaterial pm, float epsilon)
{
	SpritePolygon *sprite = new (std::nothrow) SpritePolygon();
	if (sprite && sprite->initShape(filename, pm, epsilon))
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool SpritePolygon::initShape(const char* filename, PhysicsMaterial pm, float epsilon)
{
	//获取图片多边形区域
	PolygonInfo polygon = AutoPolygon::generatePolygon(filename, Rect::ZERO, epsilon);
	//polygon创建精灵
	if (!this->initWithPolygon(polygon))
		return false;

	const auto& polygoninfo = this->getPolygonInfo();
	const auto count = polygoninfo.triangles.indexCount / 3;
	const auto indices = polygoninfo.triangles.indices;
	const auto verts = polygoninfo.triangles.verts;

	auto body = PhysicsBody::create();
	auto offset = Point::ZERO - this->getContentSize() / 2;
	for (ssize_t i = 0; i < count; i++)
	{
		//draw 3 lines
		Vec2 v2[3] = {
			Vec2(verts[indices[i * 3]].vertices.x, verts[indices[i * 3]].vertices.y),
			Vec2(verts[indices[i * 3 + 1]].vertices.x, verts[indices[i * 3 + 1]].vertices.y),
			Vec2(verts[indices[i * 3 + 2]].vertices.x, verts[indices[i * 3 + 2]].vertices.y)
		};

		body->addShape(PhysicsShapePolygon::create(v2, 3, pm, offset));
	}
	this->addComponent(body);

	return true;
}
