#include <Geode/modify/GameObject.hpp>

// most complex mod ever
class $modify(GameObject) {
    void addGlow(gd::string frame) {
		bool origEditor = m_editorEnabled;
		bool origHide = m_isHide;

        m_editorEnabled = false;
		m_isHide = false;
        GameObject::addGlow(frame);
		m_editorEnabled = origEditor;
		m_isHide = origHide;
    }
};
