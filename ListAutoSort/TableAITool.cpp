#include "TableAITool.h"

QStringList TableAITool::analyzeMixture(QString mixture, QList<FieldItem> fields)
{
	int count = fields.size(); // ����
	QStringList result;        // ���ؽ��

	// �滻�ı��е� ���š�ð�š��ٺ�
	mixture = mixture.replace(QRegExp("\\s+"), "\n");
	mixture = mixture.replace(QRegExp(QStringLiteral("[,������]")), "\n");
	QMessageBox::information(NULL, "replace", mixture);



	return result;
}
