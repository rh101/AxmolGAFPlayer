#pragma once

#include "axmol.h"
#include "GAF.h"

class BaseTest : public ax::Layer
{
public:
    virtual std::string title() const;
    virtual std::string subtitle() const;

	virtual void restartCallback(ax::Object* sender);
	virtual void nextCallback(ax::Object* sender);
	virtual void backCallback(ax::Object* sender);

    virtual void onEnter() override;
    virtual void onExit() override;
};
