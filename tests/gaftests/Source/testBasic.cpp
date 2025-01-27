#include "testBasic.h"
#include "controller.h"

TestScene::TestScene(bool bPortrait, bool physics/* = false*/)
{
    if (physics)
    {
#if AX_USE_PHYSICS
        TestScene::initWithPhysics();
#else
        Scene::init();
#endif
    }
    else
    {
        Scene::init();
    }
}

void testScene_callback(ax::Object *sender )
{
    auto scene = Scene::create();

    auto layer = new (std::nothrow) TestController();
    scene->addChild(layer);
    layer->release();

    Director::getInstance()->replaceScene(scene);
}

void TestScene::onEnter()
{
    Scene::onEnter();

    //add the menu item for back to main menu
    TTFConfig ttfConfig("fonts/arial.ttf", 54);
    auto label = Label::createWithTTF(ttfConfig,"MainMenu");

    auto menuItem = MenuItemLabel::create(label, testScene_callback );
    auto menu = Menu::create(menuItem, nullptr);

    menu->setPosition( Vec2::ZERO );
    menuItem->setPosition( Vec2( VisibleRect::right().x - 150, VisibleRect::bottom().y + 25) );

    addChild(menu, 1);
}
