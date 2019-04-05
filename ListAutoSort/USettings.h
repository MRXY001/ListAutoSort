#ifndef USETTINGS_H
#define USETTINGS_H

#include "settings.h"
#include <QColor>

/**
 * ������������
 */
class USettings : public Settings
{
public:

    USettings(QString filePath);

    void init();
    void loadFromFile();

public:
    bool is_debug;                   // ����ģʽ

};


#endif // USETTINGS_H
