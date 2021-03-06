#include "ListAutoSort.h"

ListAutoSort::ListAutoSort(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 初始化全局变量
	rt = new RuntimeInfo();
	us = new USettings(rt->DATA_PATH + "settings.ini");
	savedPath = rt->DATA_PATH + "fields.txt";

	initView();
	readFIeldsInfo();
	setTableHeader();
}

/**
* 初始化控件与连接
*/
void ListAutoSort::initView()
{
	ui.fieldsList->setEditTriggers(QAbstractItemView::DoubleClicked); // 允许双击编辑

																	  // 设置代理，处理双击编辑结束事件
	ListItemDelegate* delegate = new ListItemDelegate(this);
	ui.fieldsList->setItemDelegate(delegate);
	connect(delegate, SIGNAL(signalTextModified(int, QString)), this, SLOT(slotFieldItemTextModified(int, QString)));

	// 连接信号槽
	connect(ui.workspaceAction, SIGNAL(triggered()), this, SLOT(slotWorkSpaceActionTriggered()));
	connect(ui.resortAction, SIGNAL(triggered()), this, SLOT(slotResortButtonClicked()));
	connect(ui.helpAction, SIGNAL(triggered()), this, SLOT(slotHelpActionTriggered()));
	connect(ui.aboutAction, SIGNAL(triggered()), this, SLOT(slotAboutActionTriggered()));

	connect(ui.addCol, SIGNAL(clicked()), this, SLOT(slotFieldItemAdd()));
	connect(ui.inputButton, SIGNAL(clicked()), this, SLOT(slotInputButtonClicked()));
	connect(ui.insertButton, SIGNAL(clicked()), this, SLOT(slotInsertButtonClicked()));
	connect(ui.resortButton, SIGNAL(clicked()), this, SLOT(slotResortButtonClicked()));
	connect(ui.pasteButton, SIGNAL(clicked()), this, SLOT(slotPasteButtonClicked()));
	connect(ui.copyExcelButton, SIGNAL(clicked()), this, SLOT(slotExcelButtonClicked()));

	connect(ui.fieldsList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotFieldItemMenu(QPoint)));
	//connect(ui.fieldsList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // 无效
	//connect(ui.fieldsList, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // 无效
	connect(ui.fieldsList, SIGNAL(currentRowChanged(int)), this, SLOT(slotFieldItemRowChanged()));
	connect(ui.patternEdit, SIGNAL(textChanged(QString)), this, SLOT(slotFieldItemPatternModified(QString)));
}

/**
* 初始化保存表格
*/
void ListAutoSort::setTableHeader()
{
	ui.tableWidget->setColumnCount(fields.size());
	QStringList labels;
	for (int i = 0; i < fields.size(); i++)
		labels.append(fields[i].getName());
	ui.tableWidget->setHorizontalHeaderLabels(labels);
}

/**
* 在某一行，加上分析后的文本（添加/更新）
*/
bool ListAutoSort::setTableARow(int row, QString mixture, QList<FieldItem>fields)
{
	QStringList result = analyzeMixture(mixture, fields);

	// 识别失败
	if (result.size() != fields.size())
	{
		QString list_str = "";
		for (int i = 0; i < result.size(); i++)
		{
			if (i > 0) list_str += ", ";
			list_str += result[i];
		}
		QString all_msg = QString("Sorry, analyze failed, result is:%1").arg(list_str);
		QMessageBox::information(this, QStringLiteral("识别失败"), all_msg);
		return false;
	}

	// 添加到表格的某一行
	for (int i = 0; i < result.size(); i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(result[i]);
		ui.tableWidget->setItem(row, i, item);
	}

	return true;
}

/**
* AI识别的方法（调用 TableAITool）
*/
QStringList ListAutoSort::analyzeMixture(QString mixture, QList<FieldItem> fields)
{
	TableAITool ai(mixture, fields);

	return ai.getResult();
}

/**
* 交换字段
* * 列表上下移动
* * 表格左右移动
*/
void ListAutoSort::moveListItem(int from, int to)
{
	if (from == to)
		return;

	// 移动 QList
	fields.move(from, to);
	writeFieldsInfo(); // 保存位置修改

					   // 移动 QListWidget
	ui.fieldsList->insertItem(to, new QListWidgetItem(ui.fieldsList->item(from)->text()));
	QListWidgetItem* item = ui.fieldsList->item(from + (from > to ? 1 : 0));
	ui.fieldsList->takeItem(from + (from > to ? 1 : 0));
	delete item;
	ui.fieldsList->setCurrentRow(to); // 设置为原来的Item移动后的索引

									  // 移动 QTableWidget
	ui.tableWidget->insertColumn(to);
	if (from > to) from++;
	for (int i = 0; i < mixtures.size(); i++)
	{
		QTableWidgetItem* item = ui.tableWidget->item(i, from);
		ui.tableWidget->setItem(i, to, new QTableWidgetItem(*item));
	}
	ui.tableWidget->removeColumn(from);
	setTableHeader();
}

/**
* 交换内容
* * 表格上下移动
* * Mixtures上下移动
*/
void ListAutoSort::moveTableItem(int from, int to)
{
	if (from == to)
		return;
	if (from < to) // 下移
	{

	}
	else if (from > to) // 上移
	{

	}
}

/*
* 从保存的文件中读取
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
* 保存到文件之中
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
* 做了更改之后刷新列表
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
* TableVIew添加一列，后续可以编辑
*/
void ListAutoSort::slotFieldItemAdd()
{
	// 创建字段列表
	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("新字段"), ui.fieldsList);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui.fieldsList->editItem(item);
	fields.append(FieldItem(QStringLiteral("新字段"), ""));

	setTableHeader();

	// 保存
	writeFieldsInfo();
}

/**
* 添加 ui.inputEdit 中的内容到表格中
*/
void ListAutoSort::slotInputButtonClicked()
{
	QString mixture = ui.inputEdit->toPlainText();
	if (mixture.isEmpty()) return;

	mixtures.append(mixture);
	ui.tableWidget->setRowCount(mixtures.size()); // 设置行数（不包括标头行）
	if (setTableARow(mixtures.size() - 1, mixture, fields)) // 设置行的内容
	{
		ui.inputEdit->setPlainText("");
		ui.tableWidget->setCurrentCell(ui.tableWidget->rowCount() - 1, 0);
	}
	else // 设置失败，取消修改
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount() - 1);
		mixture.remove(mixture.size());
	}
}

/**
* 插入 ui.inputEdit 中的内容到表格中
*/
void ListAutoSort::slotInsertButtonClicked()
{
	int currentIndex = ui.tableWidget->currentRow();
	if (currentIndex < 0)
		return;

	QString mixture = ui.inputEdit->toPlainText();
	if (mixture.isEmpty()) return;

	mixtures.insert(currentIndex, mixture);
	ui.tableWidget->insertRow(currentIndex); // 通过插入一行来设置行数（不包括标头行）
	if (setTableARow(currentIndex, mixture, fields)) // 设置行的内容
	{
		ui.inputEdit->setPlainText("");
		ui.tableWidget->setCurrentCell(currentIndex, 0);
	}
	else // 设置失败，取消修改
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount() - 1);
		mixture.remove(mixture.size());
	}
}

/**
* 直接添加剪贴板中的内容到表格中
*/
void ListAutoSort::slotPasteButtonClicked()
{
	const QClipboard* clipboard = QApplication::clipboard();
	QString str = clipboard->text();
	ui.inputEdit->setPlainText(str);
	slotInputButtonClicked();
}

/**
* 全部信息重新分类（丢弃所有的修改）
* 在修改字段后适用
*/
void ListAutoSort::slotResortButtonClicked()
{
	if (ui.tableWidget->rowCount() == 0) return;
	QMessageBox::StandardButton reply =
		QMessageBox::question(this, QStringLiteral("提示"),
			QStringLiteral("请确认全部重新分拣？\n之前所做的单元格修改（如果有的话）都将取消"),
			QMessageBox::Yes | QMessageBox::Cancel);
	if (reply != QMessageBox::Yes)
		return;
	for (int i = 0; i < mixtures.size(); i++)
		setTableARow(i, mixtures[i], fields);
}

/**
* 复制表格的内容为Excel，换行+Tab
*/
void ListAutoSort::slotExcelButtonClicked()
{
	// 复制内容
	QString ans;
	for (int i = 0; i < mixtures.size(); i++)
	{
		for (int j = 0; j < fields.size(); j++)
		{
			if (j > 0) ans += "\t";
			ans += ui.tableWidget->item(i, j)->text();
		}
		ans += "\n";
	}
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(ans);

	// 是否有长数据
	bool hasLongNumber = false;
	for (int i = 0; i < fields.size(); i++)
		if (canRegExp(fields[i].getPattern(), ""))
		{
			hasLongNumber = true;
			break;
		}

	QString tip = QStringLiteral("%1行%2列 复制完毕，请到Excel对应的位置粘贴").arg(mixtures.size()).arg(fields.size());
	if (hasLongNumber)
		tip += QStringLiteral("\n\n检测到长数字字段，建议先将单元格格式设置为文本");
	QMessageBox::information(this, QStringLiteral("复制为Excel"), tip);
}

/**
* 字段右键菜单
*/
void ListAutoSort::slotFieldItemMenu(QPoint p)
{
	QListWidgetItem* item = ui.fieldsList->itemAt(p);
	if (item == NULL)
		return;

	QMenu* menu = new QMenu(this);
	QAction* renameAction = new QAction(QStringLiteral("重命名"), this);
	QAction* deleteAction = new QAction(QStringLiteral("删除"), this);
	QAction* moveUpAction = new QAction(QStringLiteral("上移"), this);
	QAction* moveDownAction = new QAction(QStringLiteral("下移"), this);
	QAction* moveTopAction = new QAction(QStringLiteral("置顶"), this);
	QAction* moveBottomAction = new QAction(QStringLiteral("置底"), this);
	menu->addAction(renameAction);
	menu->addAction(deleteAction);
	menu->addAction(moveUpAction);
	menu->addAction(moveDownAction);
	menu->addAction(moveTopAction);
	menu->addAction(moveBottomAction);
	connect(renameAction, SIGNAL(triggered()), this, SLOT(slotFieldItemRename()));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotFieldItemDelete()));
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveUp()));
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveDown()));
	connect(moveTopAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveTop()));
	connect(moveBottomAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveBottom()));

	menu->exec(QCursor::pos());
}

void ListAutoSort::slotFieldItemRename()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	ui.fieldsList->editItem(item);
}

/**
* 删除字段
*/
void ListAutoSort::slotFieldItemDelete()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	// 删除字段列表
	fields.removeAt(index);

	ui.fieldsList->takeItem(index); // 不知道为什么removeWidgetItem无效
	delete item; // takeItem 需要手动 delete

	ui.tableWidget->removeColumn(index);

	writeFieldsInfo();
}

void ListAutoSort::slotFieldItemMoveUp()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index == 0)
		return;

	moveListItem(index, index - 1);
}

void ListAutoSort::slotFieldItemMoveDown()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index >= fields.size() - 1)
		return;

	moveListItem(index, index + 1);
}

void ListAutoSort::slotFieldItemMoveTop()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index == 0)
		return;

	moveListItem(index, 0);
}

void ListAutoSort::slotFieldItemMoveBottom()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index >= fields.size() - 1)
		return;

	moveListItem(index, fields.size() - 1);
}

void ListAutoSort::slotWorkSpaceActionTriggered()
{
	QDesktopServices::openUrl(QUrl(rt->DATA_PATH));
}

/**
* 字段列表位置改变
*/
void ListAutoSort::slotFieldItemRowChanged()
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	QString pat = fields[index].getPattern();
	ui.patternEdit->setText(pat);
}

/**
* 双击修改文本结束
*/
void ListAutoSort::slotFieldItemTextModified(int row, QString text)
{
	if (row < 0 || row >= fields.size())
		return;
	if (fields[row].getName() == text)
		return;

	fields[row].setName(text);
	ui.fieldsList->setCurrentRow(row);

	if (fields[row].getPattern().isEmpty())
	{
		QString def_reg = getDefaultRegex(text);
		fields[row].setPattern(def_reg);
		if (ui.fieldsList->currentIndex().row() == row)
			ui.patternEdit->setText(def_reg);
	}

	//setTableHeader();
	QTableWidgetItem* item = ui.tableWidget->item(0, row);
	if (item == NULL) // 实测是NULL……
	{
		setTableHeader();
	}
	else
	{
		item->setText(text);
		ui.tableWidget->setHorizontalHeaderItem(row, item);
	}

	writeFieldsInfo();
}

/**
* 修改正则表达式事件
*/
void ListAutoSort::slotFieldItemPatternModified(QString text)
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	fields[index].setPattern(text);
	writeFieldsInfo();
}

/**
* 获取特定字段名的正则表达式
*/
QString ListAutoSort::getDefaultRegex(QString field)
{
	if (field == QStringLiteral("姓名"))
		return "^[^\\x00-\\xff]{2,4}$";
	if (field == QStringLiteral("性别"))
		return "[01男女]";
	if (field == QStringLiteral("年龄"))
		return "^\\d{1,2}$";
	if (field == QStringLiteral("班级"))
		return "^[^\\x00-\\xff]{2,15}\\d{2,4}班?级?$";
	if (field == QStringLiteral("手机") || field == QStringLiteral("手机号"))
		return "^1\\d{10}$";
	if (field == QStringLiteral("邮箱"))
		return "^[A-Za-z\\d]+([-_.][A-Za-z\\d]+)*@([A-Za-z\\d]+[-.])+[A-Za-z\\d]{2,4}$";
	if (field == QStringLiteral("学号"))
		return "^20\\d{10,11}$";
	if (field == QStringLiteral("身份证"))
		return "^\\d{17}.$";
	if (field == QStringLiteral("专业"))
		return "^[^\\x00-\\xff]{2,15}$";
	if (field == QStringLiteral("学校") || field == QStringLiteral("大学"))
		return "^[^\\x00-\\xff]{2,10}(大学|学校|学院)$";
	if (field == QStringLiteral("年级"))
		return "(小学|初中?|高中?|大学?|研)(一二三四)(年级)?";

	return "";
}

/**
* 帮助文本
*/
void ListAutoSort::slotHelpActionTriggered()
{
	QString str = QStringLiteral("步骤一：\n在左上方添加字段（表格标题），中间是字段的正则表达式。\n不会正则没关系，预设有姓名、手机、邮箱等。\n\n");;
	str += QStringLiteral("步骤二：\n在左下方的输入框中粘贴文本，点击“添加”，文本中的散乱信息将自动分类到表格中，无需手动调整。\n更懒的方法是，直接点“识别剪贴板”。\n\n");
	str += QStringLiteral("步骤三：\n点击右下角的“复制为Excel”，可在Excel中粘贴。\n\n后续或将支持更多骚操作，比如按顺序复制、网页表单填写等。");
	QMessageBox::information(this, QStringLiteral("AutoSortList 教程"), str);
}

/**
* 关于文本
*/
void ListAutoSort::slotAboutActionTriggered()
{
	QString str = QStringLiteral("作者：命燃芯乂\nQQ：482582886\n邮箱：wxy19980615@gmail.com\n\n");
	str += QStringLiteral("为大幅度提高信息归类效率而专门开发的办公工具\n（懒呀~~）\n\n");
	str += QStringLiteral("原创软件，允许免费传播，不作版权限制");

	QMessageBox::information(this, QStringLiteral("关于 AutoSortList"), str);
}