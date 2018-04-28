//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2018

#ifndef UISCRIPT_HPP
#define UISCRIPT_HPP

#include <huggle_core/definitions.hpp>
#include <huggle_core/script.hpp>

class QMenu;

namespace Huggle
{
    class UiScript;

    class ScriptMenu
    {
        public:
            ScriptMenu(UiScript *s, QMenu *parent, QString text, QString fc);

        private:
            QString title;
            QString callback;
            QMenu *parentMenu = nullptr;
            UiScript *script;
            QMenu *qm = nullptr;
    };

    class UiScript : public Script
    {
        public:
            static QList<UiScript*> GetAllUiScripts();
            static void Autostart();

            UiScript();
            ~UiScript();
            QString GetContext();
            unsigned int GetContextID();
            int RegisterMenu(QMenu *parent, QString title, QString fc);
        private:
            static QList<UiScript*> uiScripts;
            void registerFunctions();
            int lastMenu = 0;
            QHash<int, ScriptMenu*> scriptMenus;
    };
}

#endif // UISCRIPT_HPP
