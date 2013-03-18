﻿#include "iuiPCH.h"
#include "iuiList.h"
#include "iuiSystem.h"
#include "iuiRenderer.h"
#include "iuiUtilities.h"
namespace iui {

struct ListItem::Members
{
    String text;
    void *userdata;
    int32 index;
    bool hovered;
    bool selected;
    bool destroyed;

    Members()
        : userdata(NULL), index(0), hovered(false), selected(false), destroyed(false)
    {}
};
istMemberPtrImpl(ListItem,Members);

ListItem::ListItem(const String &text, void *userdata)
{
    m->text = text;
    m->userdata = userdata;
}

ListItem::~ListItem()
{}

void ListItem::update(Float dt)
{
}

void            ListItem::destroy()                 { m->destroyed=true; }

const String&   ListItem::getText() const           { return m->text; }
void*           ListItem::getUserData() const       { return m->userdata; }
int32           ListItem::getIndex() const          { return m->index; }
bool            ListItem::isHovered() const         { return m->hovered; }
bool            ListItem::isSelected() const        { return m->selected; }
bool            ListItem::isDestroyed() const       { return m->destroyed; }

void            ListItem::setText(const String &v)  { m->text=v; }
void            ListItem::setUserData(void *v)      { m->userdata=v; }
void            ListItem::setIndex(int32 v)         { m->index=v; }
void            ListItem::setHovered(bool v)        { m->hovered=v; }
void            ListItem::setSelected(bool v)       { m->selected=v; }


ListStyle::ListStyle()
{
    setTextHAlign(TA_HLeft);
    setTextHSpacing(0.75f);
}

void ListStyle::draw()
{
    List *w = static_cast<List*>(getWidget());
    Rect rect(Position(), w->getSize());
    TextPosition tpos(rect, getTextHAlign(), getTextVAlign(), getTextHSpacing(), getTextVSpacing());
    Color bg = getBGColor();
    iuiGetRenderer()->drawRect(rect, bg);
    iuiGetRenderer()->drawOutlineRect(rect, getBorderColor());
    Position pos;
    w->eachListItem([&](ListItem *item){
        Rect rect(pos, Size(w->getSize().x, w->getItemHeight()));
        TextPosition tpos(rect, getTextHAlign(), getTextVAlign(), getTextHSpacing(), getTextVSpacing());
        if(item->isSelected()) {
            Color ibg = vec4(1.0f, 1.0f, 1.0f, 0.4f);
            iuiGetRenderer()->drawRect(rect, ibg);
        }
        else if(item->isHovered()) {
            Color ibg = vec4(1.0f, 1.0f, 1.0f, 0.2f);
            iuiGetRenderer()->drawRect(rect, ibg);
        }
        const String &text = item->getText();
        iuiGetRenderer()->drawFont(tpos, getFontColor(), text.c_str(), text.size());
        pos.y += w->getItemHeight();
        tpos.rect.setPosition(pos);
    });
}
iuiImplDefaultStyle(List);



struct List::Members
{
    ListItemCont items;
    WidgetCallback on_item_click;
    WidgetCallback on_item_doubleclick;
    WidgetCallback on_item_hovered;
    Float item_height;
    Float scroll_pos;

    Members() : item_height(18.0f), scroll_pos(0.0f) {}
};
istMemberPtrImpl(List,Members);

Float               List::getItemHeight() const { return m->item_height; }
Float               List::getScrollPos() const  { return m->scroll_pos; }
ListItemCont&       List::getItems()            { return m->items; }
const ListItemCont& List::getItems() const      { return m->items; }

void List::setItemClickHandler(WidgetCallback cb)       { m->on_item_click=cb; }
void List::setItemDoubleClickHandler(WidgetCallback cb) { m->on_item_doubleclick=cb; }
void List::setItemHoverHandler(WidgetCallback cb)       { m->on_item_hovered=cb; }

List::List( Widget *parent, const Rect &rect, const WidgetCallback &on_item_click )
{
    setParent(parent);
    setPosition(rect.getPosition());
    setSize(rect.getSize());
    m->on_item_click = on_item_click;
}

List::~List()
{
}

void List::update(Float dt)
{
    eachListItem([&](ListItem *item){ item->setHovered(false); });

    bool hovered = false;
    HandleMouseHover(this, hovered);
    if(hovered) {
        Position pos = getPositionAbs();
        Position rel = iuiGetSystem()->getMousePos() - pos;
        int32 index = int32(rel.y / m->item_height);
        if(index>=0 && index<(int32)m->items.size()) {
            m->items[index]->setHovered(true);
            callIfValid(m->on_item_hovered);
        }
    }
    super::update(dt);
}

void List::addListItem(ListItem *item, int32 pos)
{
    // todo: pos
    m->items.push_back(item);
}

void List::addListItem(const String &text, void *userdata, int32 pos)
{
    addListItem(istNew(ListItem)(text, userdata), pos);
}

bool List::handleEvent( const WM_Base &wm )
{
    switch(MouseHitWidget(this, wm)) {
    case WH_HitMouseLeftDown:
        {
            eachListItem([&](ListItem *item){ item->setSelected(false); });

            Position pos = getPositionAbs();
            Position rel = WM_Mouse::cast(wm).mouse_pos - pos;
            int32 index = int32(rel.y / m->item_height);
            if(index>=0 && index<(int32)m->items.size()) {
                m->items[index]->setSelected(true);
                callIfValid(m->on_item_click);
            }
            setFocus(true);
            return true;
        }
    }
    return super::handleEvent(wm);
}

} // namespace iui
