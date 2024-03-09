#pragma once

#include <al/Library/Model/ModelCtrl.h>
#include <al/Library/Yaml/ByamlIter.h>

namespace al {
    struct MaterialCategoryKeeper;

    class ModelMaterialCategory {
        public:
        ModelMaterialCategory(al::ModelCtrl*, al::MaterialCategoryKeeper*);
        void init(char const*);
        void init(al::ByamlIter const&);
        void getCategoryIdFromMaterialName(char const*) const;
        void getCategoryIdFromMaterialIndex(int) const;
        static void tryCreate(al::ModelCtrl*, al::Resource const*, char const*, al::MaterialCategoryKeeper*);
        static void tryCreate(al::ModelCtrl*, char const*, al::MaterialCategoryKeeper*);
    };
}