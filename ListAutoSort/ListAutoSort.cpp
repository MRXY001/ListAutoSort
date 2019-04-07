#include "ListAutoSort.h"

ListAutoSort::ListAutoSort(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// ��ʼ��ȫ�ֱ���
	rt = new RuntimeInfo();
	us = new USettings(rt->DATA_PATH + "settings.ini");
	savedPath = rt->DATA_PATH + "fields.txt";

	initView();
	readFIeldsInfo();
	initTable();
}

/**
 * ��ʼ���ؼ�������
 */
void ListAutoSort::initView()
{
	ui.fieldsList->setEditTriggers(QAbstractItemView::DoubleClicked); // ����˫���༭

	// ���ô�������˫���༭�����¼�
	ListItemDelegate* delegate = new ListItemDelegate(this);
	ui.fieldsList->setItemDelegate(delegate);
	connect(delegate, SIGNAL(signalTextModified(int, QString)), this, SLOT(slotFieldItemTextModified(int, QString)));

	// �����źŲ�
	connect(ui.addCol, SIGNAL(clicked()), this, SLOT(slotFieldItemAdd()));
	connect(ui.inputButton, SIGNAL(clicked()), this, SLOT(slotInputButtonClicked()));
	connect(ui.pasteButton, SIGNAL(clicked()), this, SLOT(slotPasteButtonClicked()));
	connect(ui.fieldsList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotFieldItemMenu(QPoint)));
	//connect(ui.fieldsList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // ��Ч
	//connect(ui.fieldsList, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // ��Ч
	connect(ui.fieldsList, SIGNAL(currentRowChanged(int)), this, SLOT(slotFieldItemRowChanged()));
	connect(ui.patternEdit, SIGNAL(textChanged(QString)), this, SLOT(slotFieldItemPatternModified(QString)));
}

/**
 * ��ʼ��������
 */
void ListAutoSort::initTable()
{
	ui.tableWidget->setColumnCount(fields.size());
	QStringList labels;
	for (int i = 0; i < fields.size(); i++)
		labels.append(fields[i].getName());
	ui.tableWidget->setHorizontalHeaderLabels(labels);
}

/**
 * ��һ�λ������ֱܷ治ͬ����Ϣ
 */
void ListAutoSort::smartAddMixture(QString mixture)
{
	mixtures.append(mixture);
	ui.tableWidget->setRowCount(mixtures.size()); // ������������������ͷ�У�
	if (!setTableARow(mixtures.size() - 1, mixture, fields)) // �����е�����
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount()-1);
	}
}

/**
 * ��ĳһ�У����Ϸ�������ı������/���£�
 */
bool ListAutoSort::setTableARow(int row, QString mixture, QList<FieldItem>fields)
{
	QStringList result = analyzeMixture(mixture, fields);

	// ʶ��ʧ��
	if (result.size() != fields.size())
	{
		QString list_str = "";
		for (int i = 0; i < result.size(); i++)
		{
			if (i > 0) list_str += ", ";
			list_str += result[i];
		}
		QString all_msg = QString("Sorry, analyze failed, result is:%1").arg(list_str);
		QMessageBox::information(this, QStringLiteral("ʶ��ʧ��"), all_msg);
		return false;
	}

	// ��ӵ�����ĳһ��
	for (int i = 0; i < result.size(); i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(result[i]);
		ui.tableWidget->setItem(row, i, item);
	}

	return true;
}

/**
 * AIʶ��ķ��������� TableAITool��
 */
QStringList ListAutoSort::analyzeMixture(QString mixture, QList<FieldItem> fields)
{
	TableAITool ai(mixture, fields);

	return ai.getResult();
}

/*
 * �ӱ�����ļ��ж�ȡ
 */
void ListAutoSort::writeFieldsInfo()
{
	QString content;

	for (int i = 0; i < fields.size(); i++)
	{
		QString n = fields[i].getName();
		QString p = fields[i].getPattern();
		QString c = toXml(n, "name") + toXml(p, "pattern");
		content += toXml(c, "field");
	}

	writeTextFile(savedPath, content);
}

/**
 * ���浽�ļ�֮��
 */
void ListAutoSort::readFIeldsInfo()
{
	if (!isFileExist(savedPath))
		return;
	QString content = readTextFile(savedPath);

	QStringList ts = getXmls(content, "field");

	for (int i = 0; i < ts.size(); i++)
	{
		QString n = getXml(ts[i], "name");
		QString p = getXml(ts[i], "pattern");
		fields.append(FieldItem(n, p));
	}

	refreshFieldsInfo();
}

/**
 * ���˸���֮��ˢ���б�
 */
void ListAutoSort::refreshFieldsInfo()
{
	ui.fieldsList->clear();

	for (int i = 0; i < fields.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(fields[i].getName(), ui.fieldsList);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

/**
 * TableVIew���һ�У��������Ա༭
 */
void ListAutoSort::slotFieldItemAdd()
{
	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("���ֶ�"), ui.fieldsList);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	fields.append(FieldItem(QStringLiteral("���ֶ�"), ""));
	writeFieldsInfo();
}

/**
 * ��� ui.inputEdit �е����ݵ������
 */
void ListAutoSort::slotInputButtonClicked()
{
	QString str = ui.inputEdit->toPlainText();
	smartAddMixture(str);
	ui.inputEdit->setPlainText("");
}

/**
 * ֱ����Ӽ������е����ݵ������
 */
void ListAutoSort::slotPasteButtonClicked()
{
	const QClipboard* clipboard = QApplication::clipboard();
	QString str = clipboard->text();
	smartAddMixture(str);
}

/**
 * �ֶ��Ҽ��˵�
 */
void ListAutoSort::slotFieldItemMenu(QPoint p)
{
	QListWidgetItem* item = ui.fieldsList->itemAt(p);
	if (item == NULL)
		return;

	QMenu* menu = new QMenu(this);
	QAction* deleteAction = new QAction(QStringLiteral("ɾ��"), this);
	menu->addAction(deleteAction);
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotFieldItemDelete()));
	menu->exec(QCursor::pos());
}

/**
 * ɾ���ֶ�
 */
void ListAutoSort::slotFieldItemDelete()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;

	int index = ui.fieldsList->currentIndex().row();
	if (index >= 0 && index < fields.size())
		fields.removeAt(index);

	ui.fieldsList->takeItem(index); // ��֪��ΪʲôremoveWidgetItem��Ч
	delete item; // takeItem ��Ҫ�ֶ� delete
	writeFieldsInfo();
}

/**
 * �ֶ��б�λ�øı�
 */
void ListAutoSort::slotFieldItemRowChanged()
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	QString pat = fields[index].getPattern();
	//QMessageBox::information(this, QString("%1").arg(index), pat);
	ui.patternEdit->setText(pat);
}

/**
 * ˫���޸��ı�����
 */
void ListAutoSort::slotFieldItemTextModified(int row, QString text)
{
	if (row < 0 || row >= fields.size())
		return;
	if (fields[row].getName() == text)
		return;

	fields[row].setName(text);
	writeFieldsInfo();
}

/**
 * �޸�������ʽ�¼�
 */
void ListAutoSort::slotFieldItemPatternModified(QString text)
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	fields[index].setPattern(text);
	writeFieldsInfo();
}
