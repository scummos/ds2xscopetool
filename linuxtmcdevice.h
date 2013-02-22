/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TMCDEVICE_H
#define TMCDEVICE_H
#include <QtCore/QString>
#include <QtCore/QFile>
#include "abstracttmcdevice.h"

class LinuxTMCDevice : public AbstractTMCDevice {
public:
    LinuxTMCDevice(const QString& deviceFileName);
    virtual ~LinuxTMCDevice();

    virtual void write(const QByteArray& command);
    virtual QByteArray read(int bytes = -1);

private:
    QByteArray readBuffer;
    QString deviceFileName;
    QFile deviceFile;
};

#endif // TMCDEVICE_H
