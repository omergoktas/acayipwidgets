#include <QPushButton>

#include <private/qscrollarea_p.h>
#include <private/qpushbutton_p.h>

QAbstractButtonPrivate::QAbstractButtonPrivate(QSizePolicy::ControlType type)
    : shortcutId(0)
    , checkable(false)
    , checked(false)
    , autoRepeat(false)
    , autoExclusive(false)
    , down(false)
    , blockRefresh(false)
    , pressed(false)
    , group(nullptr)
    , autoRepeatDelay(300)
    , autoRepeatInterval(100)
    , controlType(type)
{}

QAbstractScrollAreaPrivate::QAbstractScrollAreaPrivate()
    :hbar(nullptr), vbar(nullptr), vbarpolicy(Qt::ScrollBarAsNeeded), hbarpolicy(Qt::ScrollBarAsNeeded),
    shownOnce(false), inResize(false), sizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored),
    viewport(nullptr), cornerWidget(nullptr), left(0), top(0), right(0), bottom(0),
    xoffset(0), yoffset(0), viewportFilter(nullptr)
{
}

QFramePrivate::QFramePrivate()
    : frect(0, 0, 0, 0),
    frameStyle(QFrame::NoFrame | QFrame::Plain),
    lineWidth(1),
    midLineWidth(0),
    frameWidth(0),
    leftFrameWidth(0), rightFrameWidth(0),
    topFrameWidth(0), bottomFrameWidth(0)
{
}

QAbstractScrollAreaPrivate::~QAbstractScrollAreaPrivate()
{
}

QFramePrivate::~QFramePrivate()
{
}

bool QAbstractScrollAreaPrivate::canStartScrollingAt(const QPoint &startPos) const
{
    Q_Q(const QAbstractScrollArea);

    // don't start scrolling on a QAbstractSlider
    if (qobject_cast<QAbstractSlider *>(q->viewport()->childAt(startPos)))
        return false;

    return true;
}

QPoint QAbstractScrollAreaPrivate::contentsOffset() const
{
    Q_Q(const QAbstractScrollArea);
    QPoint offset;
    if (vbar->isVisible())
        offset.setY(vbar->value());
    if (hbar->isVisible()) {
        if (q->isRightToLeft())
            offset.setX(hbar->maximum() - hbar->value());
        else
            offset.setX(hbar->value());
    }
    return offset;
}
