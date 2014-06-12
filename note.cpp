/*!
\file    note.cpp
\author  Nathan Robert Yee

\section LICENSE

note.cpp: Implementation file for Note class
Copyright (C) 2014  Nathan Robert Yee

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "note.h"

// A note is represented by file via a filepath to the file. The name of the
// note is identical to its respective file's name.

/*!
 * \brief Default constructor setting path of new note to "[HOME]/New Note".
 */
Note::Note()
{
    noteFilepath = QDir(getBaseNotePath() + "/New Note");
}

/*!
 * \brief Constructor setting path of new note to "path".
 *
 * \param path Absolute filepath of the location of the new note object.
 */
Note::Note(QDir path)
{
    noteFilepath = path;
}

/*!
 * \brief Returns name of notefile without preceding path information.
 *
 * \return A QString containing the name of the note.
 */
QString Note::name()
{
    return noteFilepath.absolutePath().section("/", -1, -1);
}

/*!
 * \brief Returns the path of the note.
 *
 * \return A QString representing the path to the note.
 *
 * \warning The returned path can either be absolute or relative.
 */
QString Note::path()
{
    return noteFilepath.absolutePath();
}

/*!
 * \brief Returns the contents of the note.
 *
 * \return A QString containing the contents of the note or an empty QString if
 * the read operation fails.
 */
QString Note::read()
{
    if (!(noteFilepath.path()).isEmpty() && !(noteFilepath.path().isNull())) {
        QFile file(noteFilepath.path());
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

/*!
 * \brief Write content to the note.
 *
 * If the write operation fails, no changes are made to the note.
 *
 * \param text A QString containing text to be written to the note.
 *
 * \return true if write operation succeeds, false otherwise.
 */
bool Note::write(QString text)
{
    if (!(noteFilepath.path()).isEmpty() && !(noteFilepath.path().isNull())) {
        QFile file(noteFilepath.path());
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

/*!
 * \brief Rename the note.
 *
 * If the rename operation fails, no changes are made to the note.
 *
 * \param name New name of note.
 *
 * \return true if rename operation succeeds, false otherwise.
 */
bool Note::rename(QString name)
{
    if (!(noteFilepath.path()).isEmpty() && !(noteFilepath.path().isNull())) {
        QFile file(noteFilepath.path());
        QStringList temp = noteFilepath.path().split("/");
        temp.removeLast();
        QString noteParentPath = temp.join("/");
        if (file.rename(noteFilepath.path(),
                        noteParentPath + "/" + name)) {
            if (noteFilepath.exists(noteParentPath + "/" + name)) {
                noteFilepath.setPath(noteParentPath + "/" + name);
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief Get the path of Deltanote's base directory
 *
 * The base directory of Deltanote is in "[HOME]/.deltanote".
 *
 * \return A QString representing Deltanote's base directory.
 */
QString Note::getBaseNotePath()
{
    return (QDir::homePath() + "/.deltanote");
}

/*!
 * \brief Get the path of the file containing Deltanote's last-used-note
 * information.
 *
 * The file is in "[HOME]/.config/deltanote/lastnote".
 *
 * \return A QString representing the path of the last-used-note.
 */
QString Note::getLastNoteSettingsPath()
{
    return (QDir::homePath() + "/.config/deltanote/lastnote");
}
