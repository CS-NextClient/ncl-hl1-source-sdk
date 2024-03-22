#pragma once

#include "IScheme.h"
#include "Color.h"
#include <vgui_border.h>
#include <tier1/utlvector.h>
#include <unordered_map>
#include <string>

class SchemeEx : public vgui2::IScheme
{
    struct SchemeBorder_t
    {
        vgui2::Border *border;
        int borderSymbol;
        bool bSharedBorder;
    };
    CUtlVector<SchemeBorder_t> border_list_;
    std::unordered_map<std::string, vgui2::Border*> borders_map_;

    vgui2::IScheme* scheme_;
    std::string path_;

public:
    explicit SchemeEx(vgui2::IScheme* scheme, std::string path);
    ~SchemeEx() override;

    const char *GetResourceString(const char *stringName) override;
    vgui2::IBorder *GetBorder(const char *borderName) override;
    vgui2::HFont GetFont(const char *fontName, bool proportional) override;
    Color GetColor(const char *colorName, Color defaultColor) override;

private:
    void LoadBorders();
};

