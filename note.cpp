//    note.cpp: Source file for Note class
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

#include "note.h"

/**
 * @brief note::note
 *
 * A note is represented by file via a filepath to the file. The name of the
 * note is identical to its respective file's name.
 */

Note::Note()
{
    noteFilepath = getBaseNotePath() + "/Note";
}

// Note path starts at BaseNotePath (e.g. pass in "Note" to set filepath to
// "[BaseNotePath]/Note"
Note::Note(QString pathToNote) {
    noteFilepath = getBaseNotePath() + pathToNote;
}

// Returns name of notefile without preceding path
QString Note::name() {
    return noteFilepath.section("/", -1, -1);
}

QString Note::filepath() {
    return noteFilepath;
}

// Returns empty string on error
QString Note::read() {
    if (!noteFilepath.isEmpty() && !noteFilepath.isNull()) {
        QFile file(noteFilepath);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream ts(&file);
            QString readContent = ts.readAll();
            ts.flush();
            file.close();
            return readContent;
        }
    }
    return "";
}

// Abort write and return false on error
bool Note::write(QString text) {
    if (!noteFilepath.isEmpty() && !noteFilepath.isNull()) {
        QFile file(noteFilepath);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream ts(&file);
            ts << text;
            ts.flush();
            file.close();
            return true;
        }
    }
    return false;
}

// Abort rename on error
bool Note::rename(QString name) {
    if (!noteFilepath.isEmpty() && !noteFilepath.isNull()) {
        QFile file(noteFilepath);
        if (file.rename(name)) {
            return true;
        }
    }
    return false;
}

// BaseNotePath is [HOME]/.deltanote
QString Note::getBaseNotePath() {
    return (QDir::homePath() + "/.deltanote");
}

QString Note::getLastNoteSettingsPath() {
    return (QDir::homePath() + "/.config/deltanote/lastnote");
}