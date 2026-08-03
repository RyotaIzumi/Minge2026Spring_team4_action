#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

namespace Iwanna {

    class StockNearGameObjects {
    public:
        int cellSize = 64;
        HashTable<Point, Array<GameObject*>> cells;

        void clear() {
            // セルと各配列の確保領域は再利用し、毎フレームの再確保を避ける
            for (auto& [cell, objects] : cells) {
                objects.clear();
            }
        }

        Point getCell(const Vec2& pos) const {
            return Point(
                static_cast<int32>(Floor(pos.x / cellSize)),
                static_cast<int32>(Floor(pos.y / cellSize))
            );
        }

        void add(GameObject* obj) {
            const RectF area = obj->getBroadRect();
            const Point tl = getCell(area.pos);
            const Point br = getCell(area.pos + area.size);

            for (int y = tl.y; y <= br.y; ++y) {
                for (int x = tl.x; x <= br.x; ++x) {
                    cells[Point(x, y)].push_back(obj);
                }
            }
        }

        Array<GameObject*> query(const RectF& area) {
            Array<GameObject*> result;
            HashSet<GameObject*> addedObjects;
            Point tl = getCell(area.pos);
            Point br = getCell(area.pos + area.size);

            for (int y = tl.y; y <= br.y; ++y) {
                for (int x = tl.x; x <= br.x; ++x) {
                    if (cells.contains(Point(x,y))) {
                        for (auto* o : cells[Point(x,y)]) {
                            if (addedObjects.contains(o)) {
                                continue;
                            }
                            addedObjects.insert(o);
                            result.push_back(o);
                        }
                    }
                }
            }
            return result;
        }
    };
}
