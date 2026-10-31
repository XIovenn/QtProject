#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet(R"(
    QMainWindow {
        background-color: #f4f6ec;
    }

    QLabel#lblExpression {
        font-family: "SimSun";
        font-size: 16px;
        color: #666666;
        background-color: #f4f6ec;
        border: none;
    }

    QLineEdit#display {
        font-family: "SimSun";
        font-size: 90px;
        color: #000000;
        background-color: #f4f6ec;
        border: none;
    }

    QPushButton {
        font-family: "SimSun";
        font-size: 24px;
        color: #000000;
        background-color: #f4f6ec;
        border: 1px solid #e4e4dc;
        border-radius: 6px;
    }

    QPushButton:hover {
        background-color: #e9ebe0;
    }

    QPushButton:pressed {
        background-color: #dcded2;
    }

    QPushButton#btnEqual {
        background-color: #4158e8;
        color: white;
        border: none;
    }

    QPushButton#btnEqual:hover {
        background-color: #3448d8;
    }

    QPushButton#btnEqual:pressed {
        background-color: #293ac2;
    }

    QLabel#labelTip {
        font-family: "SimSun";
        font-size: 14px;
        color: #333333;
        background-color: #f4f6ec;
        border: none;
    }
    )");


    // 如果你确定 display 也是 QLineEdit，保留这两行，如果也是 QLabel，请删掉 setReadOnly
    if(ui->display->metaObject()->className() == QString("QLineEdit")){
        ui->display->setReadOnly(true);
    }
    ui->display->setFocusPolicy(Qt::NoFocus);

    // lblExpression 是 QLabel，直接设置不获取焦点即可，删掉 setReadOnly
    ui->lblExpression->setFocusPolicy(Qt::NoFocus);

    ui->lblExpression->setText("");
    ui->display->setText("0");


    digitBTNS={
        {Qt::Key_0,ui->btnNum0},
        {Qt::Key_1,ui->btnNum1},
        {Qt::Key_2,ui->btnNum2},
        {Qt::Key_3,ui->btnNum3},
        {Qt::Key_4,ui->btnNum4},
        {Qt::Key_5,ui->btnNum5},
        {Qt::Key_6,ui->btnNum6},
        {Qt::Key_7,ui->btnNum7},
        {Qt::Key_8,ui->btnNum8},
        {Qt::Key_9,ui->btnNum9},
        {Qt::Key_Plus,ui->btnPlus},
        {Qt::Key_Minus,ui->btnMinus},
        {Qt::Key_Asterisk,ui->btnMultiple},
        {Qt::Key_Slash,ui->btnDivide},
        {Qt::Key_Percent,ui->btnPercentage},
    };

    for(int k = Qt::Key_0; k <= Qt::Key_9; ++k){
           if(digitBTNS.contains(k))
               connect(digitBTNS[k], SIGNAL(clicked()), this, SLOT(btnNumClicked()));
       }

    //数字按钮单独绑定数字槽
//    connect(ui->btnNum0, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum1, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum2, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum3, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum4, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum5, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum6, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum7, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum8, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

//    connect(ui->btnNum9, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    // 二元运算符绑定二元槽
    connect(ui->btnMultiple, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMinus, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnDivide, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnPlus, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));

    // 一元运算符
    connect(ui->btnPercentage, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnInverse, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));

    connect(ui->btnSign, SIGNAL(clicked()),this,SLOT(btnSignClicked()));
    connect(ui->btnClear, SIGNAL(clicked()),this,SLOT(on_btnClear_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateExpressionUI()
{
    ui->lblExpression->setText(expressionStr);
}

QString MainWindow::calculation(bool *ok)
{
    double result=0.0;
    if(operands.size()==2 && opcodes.size()>0)
    {
        //取操作数
        double operand2 = operands.top().toDouble();
        operands.pop();
        double operand1 = operands.top().toDouble();
        operands.pop();

        //取操作符
        QString op = opcodes.top();
        opcodes.pop();

        //运算
        if (op == "+"){
            result = operand1 + operand2;
        }else if(op == "-"){
            result = operand1 - operand2;
        }else if(op == "*" || op == "×")
            result = operand1 * operand2;
        else if(op == "/" || op == "÷"){
            if(fabs(operand2) < 1e-9)
            {
                ui->statusbar->showMessage("除零错误");
                if(ok) *ok = false;
                return "Error";
            }
            result = operand1 / operand2; // 补上缺失的除法赋值
        }
        operands.push(QString::number(result));

        ui->statusbar->showMessage(QString("Calculation's in process,Counts of operands is %1,"
        "Counts of opcode is %2").arg(operands.size()).arg(opcodes.size()));
    }
    else
    {
        ui->statusbar->showMessage(QString("Counts of operands is %1, Counts of opcode is %2").
        arg(operands.size()).arg(opcodes.size()));
    }
    if(ok) *ok = true;
    return operands.isEmpty() ? operand : operands.top();
}



void MainWindow::btnNumClicked()
{
    if(isNewEntry) {
            operand.clear();
            expressionStr.clear();
            updateExpressionUI();
            isNewEntry = false;
        }

    QString digit=qobject_cast<QPushButton *>(sender())->text();

    if (operand == "0") {
          if (digit == "0") return; // 如果已经是 0，且输入还是 0，直接忽略
          operand.clear();          // 如果已经是 0，输入别的数字，先清空
      } else if (operand.isEmpty()) {
          if (digit == "0") {
              operand = "0";        // 如果是空，输入 0，正常显示 0
              ui->display->setText(operand);
              return;
          }
      }
    operand +=digit;
    ui->display->setText(operand);
}

void MainWindow::on_btnPeriod_clicked()
{
    if(isNewEntry) {
            operand.clear();
            isNewEntry = false;
        }
        if(!operand.contains(".")) {
            if(operand.isEmpty()) operand = "0";
            operand += ".";
        }
        ui->display->setText(operand);
}

void MainWindow::on_btnDel_clicked()
{
    if(isNewEntry) return; // 结果不允许退格
    if(operand.isEmpty()) return;
    operand = operand.left(operand.length()-1);
    ui->display->setText(operand);
}

void MainWindow::on_btnClearAll_clicked()
{
    operand.clear();
    operands.clear();
    opcodes.clear();
    expressionStr.clear();
    isNewEntry = false;

    ui->display->setText(operand);
    updateExpressionUI();
    ui->statusbar->showMessage("C：全部清空");
}

// CE：清除当前输入，栈里面已经存好的数字、运算符不动！
void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    isNewEntry = false;
    ui->display->setText(operand);
    ui->statusbar->showMessage("CE：清空当前输入");
}

// +/- 正负切换
void MainWindow::btnSignClicked()
{
    if(operand.isEmpty())
        return;

    double val = operand.toDouble();
    val = -val;
    operand = QString::number(val, 'g', 10);
    ui->display->setText(operand);
}

void MainWindow::btnBinaryOperatorClicked()
{
//    ui->statusbar->showMessage("last operand"+ operand);
//    QString opcode = qobject_cast<QPushButton *>(sender())->text();
//    qDebug()<<opcode;

    QString opcode = qobject_cast<QPushButton *>(sender())->text();
       if(opcode == "×") opcode = "*";
       if(opcode == "÷") opcode = "/";

       if(isNewEntry) {
              expressionStr = operand + " " + opcode + " ";
              operands.push(operand);
              opcodes.push(opcode);
              operand = "";
              isNewEntry = false;
              ui->display->setText("0");
              updateExpressionUI();
              return;
          }

       if (operand != ""){
               operands.push(operand);
               expressionStr += operand + " " + opcode + " "; // 拼接前端显示算式
               operand = "";
               opcodes.push(opcode);

               // 【修复3】：连续运算，如果栈里有两个数，立刻计算前一步
               if (operands.size() >= 2) {
                   bool ok;
                   QString res = calculation(&ok);
                   if(!ok) {
                       ui->display->setText("Error");
                       return;
                   }

                   ui->display->setText(res);
                   // 计算后，上面的算式变成 "结果 + "
                   expressionStr = res + " " + opcode + " ";

               } else {
                   ui->display->setText("0");
               }

           } else if (!expressionStr.isEmpty()) {
               // 连续按运算符（如 1 + 再按 *），替换最后面的运算符
               expressionStr.chop(3);
               expressionStr += opcode + " ";
               opcodes.pop(); // 弹出栈顶旧的运算符
               opcodes.push(opcode); // 压入新的运算符
           }

        updateExpressionUI();
}

void MainWindow::btnUnaryOperatorClicked()
{
    bool err = false;
    QString op = qobject_cast<QPushButton*>(sender())->text();

    if (isNewEntry) {
            expressionStr.clear();
            isNewEntry = false;
        }

    if (operand != ""){
        double result = operand.toDouble();
        QString origOperand = operand;

        if(op == "%")
            result /=100.0;

        else if(op == "1/x")
        {
            if(fabs(result) < 1e-9)
            {
                ui->statusbar->showMessage("错误：0不能取倒数");
                err = true;
            }
            else
                result =1/result;
        }

        else if(op == "x²" || op == "x^2")
             result *= result;

       else if(op == "√x" || op == "√"){
            if(result < 0)
            {
                ui->statusbar->showMessage("错误：负数不能开根号");
                err = true;
            }
            else
                result = sqrt(result);
        }
        if(err)
        {
            ui->display->setText("Error");
            operand.clear();
            return;
        }

        expressionStr += (op == "1/x") ? QString("1/(%1)").arg(operand) :
                         QString("(%1)%2").arg(operand).arg(op);

        operand = QString::number(result);
        ui->display->setText(operand);
        updateExpressionUI();
        isNewEntry = true;
    }
}

void MainWindow::on_btnEqual_clicked()
{
    // 【核心修复】：如果当前已经按过等号了，再次按等号直接忽略，不进行任何追加
    if (isNewEntry) {
        return;
    }

    if (operand != ""){
        operands.push(operand);
        expressionStr += operand + " ";
        operand = "";
    } else if (!operands.isEmpty() && !opcodes.isEmpty()) {
        operands.push(operands.top());
        expressionStr += operands.top() + " ";
    }

    expressionStr += "=";
    updateExpressionUI();

    while (operands.size() >= 2 && !opcodes.isEmpty()) {
        bool ok;
        QString res = calculation(&ok);
        if(!ok) {
            ui->display->setText("Error");
            return;
        }
        ui->display->setText(res);
    }

    if (!operands.isEmpty()) {
        operand = operands.top();
        operands.clear();
        opcodes.clear();
        ui->display->setText(operand);
        isNewEntry = true; // 标记：已按过等号
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 【修复】：优先通过 event->text() 判断，这样主键盘上的 Shift+= 也能正确识别为 "+"
    QString txt = event->text();

    if (txt == "+") { ui->btnPlus->animateClick(100); return; }
    if (txt == "-") { ui->btnMinus->animateClick(100); return; }
    if (txt == "*") { ui->btnMultiple->animateClick(100); return; }
    if (txt == "/") { ui->btnDivide->animateClick(100); return; }
    if (txt == "%") { ui->btnPercentage->animateClick(100); return; }
    if (txt == ".") { ui->btnPeriod->animateClick(100); return; }

    // 如果按的是 "=" 字符，或者原生 key() 是等于键
    if (txt == "=" || event->key() == Qt::Key_Equal) {
        ui->btnEqual->animateClick(100);
        return;
    }

    // 回车键等号
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        ui->btnEqual->animateClick(100);
        return;
    }
    // 退格键
    if (event->key() == Qt::Key_Backspace) {
        ui->btnDel->animateClick(100);
        return;
    }
    // 清空键
    if (event->key() == Qt::Key_Escape) {
        ui->btnClearAll->animateClick(100);
        return;
    }
    if (event->key() == Qt::Key_Delete) {
        ui->btnClear->animateClick(100);
        return;
    }

    // 剩下的走常规映射 (数字 0-9)
    foreach(auto btnKey , digitBTNS.keys()){
        if(event->key() == btnKey) {
            digitBTNS[btnKey]->animateClick(100);
            return; // 匹配到直接返回
        }
    }

    QMainWindow::keyPressEvent(event);
}
