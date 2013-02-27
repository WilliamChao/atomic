﻿#include "iuiPCH.h"
#include "iuiSystem.h"
#include "iuiWidget.h"
#include "iuiWindow.h"
#include "iuiRenderer.h"
namespace iui {


UISystem * UISystem::s_inst;

void UISystem::initializeInstance()
{
    if(s_inst==NULL) {
        s_inst = istNew(UISystem)();
    }
}

void UISystem::finalizeInstance()
{
    istSafeDelete(s_inst);
}

UISystem* UISystem::getInstance()
{
    return s_inst;
}


struct UISystem::Members
{
    typedef UISystem::WMHandler WMHandler;

    UIRenderer  *renderer;
    Widget      *root_widget;
    Widget      *focus;
    WMHandler   wmhandler;
    Rect        screen;

    Members()
        : renderer(NULL)
        , root_widget(NULL)
        , focus(NULL)
        , screen()
    {
    }
};

UIRenderer* UISystem::getRenderer() const       { return m->renderer; }
Widget*     UISystem::getRootWidgets() const    { return m->root_widget; }
Widget*     UISystem::getFocus() const          { return m->focus; }
const Rect& UISystem::getScreen() const         { return m->screen; }

void UISystem::setScreen( float32 width, float32 height )
{
    m->screen = Rect(Position(), Size(width, height));
    m->root_widget->setPosition(m->screen.pos);
    m->root_widget->setSize(m->screen.size);
}

void UISystem::setScreen( float32 left, float32 right, float32 bottom, float32 top )
{
    m->screen = Rect(Position(left, top), Size(right-left, bottom-top));
}


UISystem::UISystem()
{
    m->root_widget = istNew(RootWindow)();
    m->renderer = CreateUIRenderer();
    m->wmhandler = std::bind(&UISystem::handleWindowMessage, this, std::placeholders::_1);

    ist::Application *app = istGetAplication();
    uvec2 wsize = app->getWindowSize();
    app->addMessageHandler(&m->wmhandler);
    setScreen((float32)wsize.x, (float32)wsize.y);
}

UISystem::~UISystem()
{
    istGetAplication()->eraseMessageHandler(&m->wmhandler);
    istSafeRelease(m->renderer);
    istSafeRelease(m->root_widget);
}

bool UISystem::sendWindowMessage( Widget *widget, const WM_Base &wm )
{
    bool handled = false;
    widget->eachChildren([&](Widget *c){
        if(!handled && sendWindowMessage(c, wm)) { handled=true; }
    });
    if(!handled && widget->handleEvent(wm)) { handled=true; }
    return handled;
}

bool UISystem::handleWindowMessage( const ist::WM_Base &wm )
{
    if(m->root_widget) {
        sendWindowMessage(m->root_widget, wm);
    }
    return false;
}

void UISystem::update( Float dt )
{
    if(m->root_widget) {
        m->root_widget->update(dt);
    }
}

void UISystem::draw()
{
    if(m->root_widget) {
        m->root_widget->draw();
    }
}

void UISystem::setFocus( Widget *v )
{
    if(m->focus) {
        // todo defocus
    }
    m->focus = v;
    if(m->focus) {
        // todo focus
    }
}


} // namespace iui
