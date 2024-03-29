#include "stdafx.h"

CBaseMenu::CBaseMenu() :
    m_MenuHandle((HMENU)CreateMenu())
{
}

bool CBaseMenu::AddMenu(HMENU hMenu, MenuItemList Items)
{
    if (Items.begin() == Items.end())
    {
        return false;
    }

    UINT ItemID, uFlags;
    std::wstring Text, String;
    for (MenuItemList::iterator MenuItem = Items.begin(); MenuItem != Items.end(); MenuItem++)
    {
        ItemID = MenuItem->ID();
        uFlags = MF_STRING;
        Text = wGS(MenuItem->Title());

        if (MenuItem->Title() == EMPTY_STRING && MenuItem->ManualString().length() > 0)
        {
            Text = MenuItem->ManualString();
        }
        if (ItemID == SPLITER)
        {
            uFlags |= MF_SEPARATOR;
        }
        if (MenuItem->ItemTicked())
        {
            uFlags |= MFS_CHECKED;
        }
        if (MenuItem->ItemEnabled())
        {
            uFlags |= MFS_ENABLED;
        }
        else
        {
            uFlags |= MFS_DISABLED;
        }

        MenuItemList * SubMenu = (MenuItemList *)MenuItem->SubMenu();
        if (ItemID == SUB_MENU && HIWORD(SubMenu) != 0 && (SubMenu->begin() != SubMenu->end()))
        {
            ItemID = (UINT)((LONG_PTR)CreatePopupMenu());
            uFlags |= MF_POPUP;

            AddMenu((HMENU)((LONG_PTR)ItemID), *SubMenu);
        }

        if (ItemID == ID_PLUGIN_MENU)
        {
            ItemID = (UINT)((LONG_PTR)MenuItem->SubMenu());
            uFlags |= MF_POPUP;
            MENUITEMINFO lpmii;

            lpmii.cbSize = sizeof(MENUITEMINFO);
            lpmii.fMask = MIIM_STATE;
            lpmii.fState = 0;
            SetMenuItemInfo((HMENU)((LONG_PTR)ItemID), (DWORD)((LONG_PTR)MenuItem->SubMenu()), FALSE, &lpmii);
        }

        if (MenuItem->ShortCut().empty() == false)
        {
            String = Text;
            String += L"\t";
            String += MenuItem->ShortCut();
            Text = String;
        }
        AppendMenu(hMenu, uFlags, ItemID, Text.c_str());
    }
    return true;
}
