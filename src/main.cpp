#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

bool g_isGlowEnabled;

$on_mod(Loaded) {
    g_isGlowEnabled = geode::Mod::get()->getSavedValue<bool>("glow-enabled", true);
}

// most complex mod ever
class $modify(GameObject) {
    void addGlow(gd::string frame) {
        if (!g_isGlowEnabled) {
            GameObject::addGlow(frame);
            return;
        }

        auto gm = GameManager::get();

        bool origEditor = m_editorEnabled;
        bool origHide = m_isHide;

        m_editorEnabled = false;
        m_isHide = false;
        GameObject::addGlow(frame);
        m_editorEnabled = origEditor;
        m_isHide = origHide;
    }
};

class $modify(LevelEditorLayer) {
    GameObject* createObject(int obj, cocos2d::CCPoint pos, bool p2) {
        auto ret = LevelEditorLayer::createObject(obj, pos, p2);

        if (ret && g_isGlowEnabled) ret->addGlow(ObjectToolbox::sharedState()->m_allKeys[obj]);

        return ret;
    }
};

class $modify(HookedEditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorPauseLayer::init(editor)) return false;

        auto menu = geode::cast::typeinfo_cast<cocos2d::CCMenu*>(getChildByID("options-menu"));
        if (!menu) return true;

        menu->setScale(.95f);
        menu->setPositionY(menu->getPositionY() - 10.f);
        menu->setPositionX(menu->getPositionX() - 8.f);

        // if someone can pr a solution to make this use the node ids layout
        // that would be nice - right now the node ids layout is improperly set
        // up to add more toggles like this
        // would also mean i dont have to move around the positioning and scale
        // of menus and stuff
        auto toggler = GameToolbox::createToggleButton(
            "Show Object Glow*",
            menu_selector(HookedEditorPauseLayer::onToggleObjectGlow),
            g_isGlowEnabled,
            menu, { 19.159f, 270.962f },
            this, this,
            .55f, .42f, 85.f,
            { 5.f, 0.f },
            "bigFont.fnt",
            false, 0, nullptr
        );
        
        toggler->setID("toggler"_spr);

        auto infoMenu = getChildByID("info-menu");
        if (!infoMenu) return true;

        auto lengthLabel = infoMenu->getChildByID("length-label");
        if (!lengthLabel) return true;

        lengthLabel->setPositionY(lengthLabel->getPositionY() + 3.f);

        return true;
    }

    void onToggleObjectGlow(cocos2d::CCObject* sender) {
        g_isGlowEnabled = !g_isGlowEnabled;
        geode::Mod::get()->setSavedValue("glow-enabled", g_isGlowEnabled);

        if (!geode::Mod::get()->getSavedValue<bool>("popup-shown", false)) {
            FLAlertLayer::create(
                "Notice",
                "You must <co>exit</c> and <cy>re-enter</c> the editor to reload <cj>object glow</c>!",
                "ok"
            )->show();
            geode::Mod::get()->setSavedValue("popup-shown", true);
        }
    }
};
