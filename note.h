//    note.h: Header file for Note class
//    Copyright (C) 2014  Nathan Robert Yee

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef NOTE_H
#define NOTE_H

#include <QFileSystemModel>
#include <QTextStream>

class Note
{
public:
    Note();
    Note(QString pathToNote);
    QString name();
    QString filepath();
    QString read();
    bool write(QString text);
    bool rename(QString name);

private:
    QString noteFilepath;

    QString getBaseNotePath();
    QString getLastNoteSettingsPath();
};

#endif // NOTE_H
