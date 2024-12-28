#include <QMouseEvent>
#include <QPainter>
#include "mblineedit.h"

const int DEFAULT_MARGIN = 10;

MbLineEdit::MbLineEdit(QWidget *parent)
    : QWidget{parent},
    m_cursorPosition(0),
    m_text("")
{
    // Initialize timer
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MbLineEdit::blinkCursor);
        timer->start(500);
    }

    isSelected = false;
    setFocusPolicy(Qt::StrongFocus);
}

int MbLineEdit::getCursorPosition() const
{
    return m_cursorPosition;
}

QString MbLineEdit::getText() const
{
    return m_text;
}

void MbLineEdit::setCursorPosition(int cursorPosition)
{
    m_cursorPosition = cursorPosition;
}

void MbLineEdit::setText(const QString &text)
{
    m_text = text;
}

/* protected */

void MbLineEdit::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Left:
        {
            if(m_cursorPosition > 0)
            {
                m_cursorPosition--;
                update();
            }
            break;
        }
        case Qt::Key_Right:
        {
            if(m_cursorPosition < m_text.length())
            {
                m_cursorPosition++;
                update();
            }
            break;
        }
        default:
        {
            bool isKeyPrintable = !event->text().isEmpty() && event->text().at(0).isPrint();
            if(isKeyPrintable)
            {
                m_text.insert(m_cursorPosition++, event->text());
            }
        }
    }
}

QSize MbLineEdit::minimumSizeHint() const
{
    int height = 12 + 5 + 5;
    int width = rect().width();
    return QSize(width, height);
}

void MbLineEdit::mousePressEvent(QMouseEvent *event)
{
    // Determine which character I pressed.
    {
        int x = event->pos().x();
        int index = charIndexAt(x);
        m_cursorPosition = index;
    }

    isSelected = true;
    update();
}

void MbLineEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Draw border.
    {
        if(isSelected)
        {
            painter.setPen(QPen(Qt::blue, 4));
        }
        else if(!isSelected)
        {
            painter.setPen(QPen(Qt::black, 2));
        }
        painter.drawRect(rect());
    }

    QFont font("Arial", 12);
    painter.setFont(font);
    QFontMetrics metrics(font);
    int maxWidth = width();
    QString elidedText = metrics.elidedText(m_text, Qt::ElideRight, maxWidth);

    // Draw text.
    {
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, elidedText);
    }

    // Draw text cursor.
    {
        if(isSelected && isCursorVisible)
        {
            int cursorX = metrics.horizontalAdvance(elidedText.left(m_cursorPosition));
            int cursorY = (height() - metrics.height()) / 2;
            int cursorHeight = metrics.height();
            painter.drawLine(cursorX, cursorY, cursorX, cursorY + cursorHeight);
        }
    }
}

QSize MbLineEdit::sizeHint() const
{
    int height = 12 + 5 + 5;
    int width = rect().width();
    return QSize(width, height);
}

/* private slots */

void MbLineEdit::blinkCursor()
{
    isCursorVisible = !isCursorVisible;
    update();
}

/* private */

int MbLineEdit::charIndexAt(int x)
{
    QFont font("Arial", 12);
    QFontMetrics metrics(font);
    int widthAccumulated = 0;

    for(int i = 0; i < m_text.length(); i++)
    {
        widthAccumulated += metrics.horizontalAdvance(m_text[i]);
        if(x < widthAccumulated)
        {
            return i;
        }
    }

    return m_text.length();
}
