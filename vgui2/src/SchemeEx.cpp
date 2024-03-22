#include <vgui/SchemeEx.h>
#include <vgui_border.h>
#include <KeyValues.h>
#include <vgui_controls/Controls.h>

SchemeEx::SchemeEx(vgui2::IScheme* scheme, std::string path) :
    scheme_(scheme),
    path_(std::move(path))
{
}

SchemeEx::~SchemeEx()
{
    for (auto& it : borders_map_)
    {
        delete it.second;
    }
}

const char *SchemeEx::GetResourceString(const char *stringName)
{
    return scheme_->GetResourceString(stringName);
}

vgui2::IBorder *SchemeEx::GetBorder(const char *borderName)
{
    if (border_list_.IsEmpty())
        LoadBorders();

    if (borders_map_.count(borderName) == 0)
        return scheme_->GetBorder(borderName);

    return borders_map_[borderName];
}

vgui2::HFont SchemeEx::GetFont(const char *fontName, bool proportional)
{
    return scheme_->GetFont(fontName, proportional);
}

Color SchemeEx::GetColor(const char *colorName, Color defaultColor)
{
    return scheme_->GetColor(colorName, defaultColor);
}

void SchemeEx::LoadBorders()
{
    auto data = new KeyValues("Scheme");
    data->LoadFromFile(g_pFullFileSystem, path_.c_str());

    auto borders = data->FindKey("Borders", true);
    {
        for (KeyValues *kv = borders->GetFirstSubKey(); kv != nullptr; kv = kv->GetNextKey())
        {
            if (kv->GetDataType() == KeyValues::TYPE_STRING)
            {
                // it's referencing another border, ignore for now
            }
            else
            {
                int i = border_list_.AddToTail();

                auto *border = new vgui2::Border();
                border->SetName(kv->GetName());
                border->ApplySchemeSettings(scheme_, kv);

                border_list_[i].border = border;
                border_list_[i].bSharedBorder = false;
                border_list_[i].borderSymbol = kv->GetNameSymbol();

                borders_map_.emplace(kv->GetName(), border);
            }
        }
    }

    // iterate again to get the border references
    for (KeyValues *kv = borders->GetFirstSubKey(); kv != nullptr; kv = kv->GetNextKey())
    {
        if (kv->GetDataType() == KeyValues::TYPE_STRING)
        {
            // it's referencing another border, find it
            auto *border = (vgui2::Border *)GetBorder(kv->GetString());
//			const char *str = kv->GetName();
            Assert(border);

            // add an entry that just references the existing border
            int i = border_list_.AddToTail();
            border_list_[i].border = border;
            border_list_[i].bSharedBorder = true;
            border_list_[i].borderSymbol = kv->GetNameSymbol();

            borders_map_.emplace(kv->GetName(), border);
        }
    }

    data->deleteThis();
}
