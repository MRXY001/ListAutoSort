/**
 * ȫ�ֱ���
 */

#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QCoreApplication>
#include "usettings.h"
#include "runtimeinfo.h"

// ==== Ӧ�� ====
extern QString APPLICATION_NAME;
extern QString VERSION_NUMBER;

extern RuntimeInfo* rt;
extern USettings* us;
extern QString Path(QString str);
extern qint64 getTimestamp();

void initGlobal();
void deleteGlobal();

#endif // GLOBALVAR_H
