import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Private 1.0

/*!
    \qmltype TumblerColumn
    \inqmlmodule QtQuick.Extras
    \since 5.5
    \ingroup extras
    \brief A column within a tumbler.

    TumblerColumn represents a column within a tumbler, providing the interface
    to define the items and width of each column.

    \code
    Tumbler {
        TumblerColumn {
            model: [1, 2, 3]
        }

        TumblerColumn {
            model: ["A", "B", "C"]
            visible: false
        }
    }
    \endcode

    You can create a custom appearance for a Tumbler by assigning a
    \l {TumblerStyle}.
*/

QtObject {
    id: tumblerColumn

    /*! \internal */
    property Item __tumbler: null

    /*!
        \internal

        The index of this column within the tumbler.
    */
    property int __index: -1

    /*!
        \internal

        The index of the current item, if the PathView has items instantiated,
        or the last current index if it doesn't.
    */
    property int __currentIndex: -1

    Accessible.role: Accessible.ColumnHeader

    /*!
        \qmlproperty readonly int TumblerColumn::currentIndex

        This read-only property holds the index of the current item for this
        column. If the model count is reduced, the current index will be
        reduced to the new count minus one.

        \sa {Tumbler::currentIndexAt}, {Tumbler::setCurrentIndexAt}
    */
    readonly property alias currentIndex: tumblerColumn.__currentIndex

    /*!
        This property holds the model that provides data for this column.
    */
    property var model: null

    /*!
        This property holds the model role of this column.
    */
    property string role: ""

    /*!
        The item delegate for this column.

        If set, this delegate will be used to display items in this column,
        instead of the
        \l {TumblerStyle::}{delegate}
        property in \l {TumblerStyle}.

        The \l {Item::implicitHeight}{implicitHeight} property must be set,
        and it must be the same for each delegate.
    */
    property Component delegate

    /*!
        The highlight delegate for this column.

        If set, this highlight will be used to display the highlight in this
        column, instead of the
        \l {TumblerStyle::}{highlight}
        property in \l {TumblerStyle}.
    */
    property Component highlight

    /*!
        The foreground of this column.

        If set, this component will be used to display the foreground in this
        column, instead of the
        \l {TumblerStyle::}{columnForeground}
        property in \l {TumblerStyle}.
    */
    property Component columnForeground

    /*!
        This property holds the visibility of this column.
    */
    property bool visible: true

    /*!
        This read-only property indicates whether the item has active focus.

        See Item's \l {Item::activeFocus}{activeFocus} property for more
        information.
    */
    readonly property bool activeFocus: {
        if (__tumbler === null)
            return null;

        var view = __tumbler.__viewAt(__index);
        return view && view.activeFocus ? true : false;
    }

    /*!
        This property holds the width of this column.
    */
    property real width: TextSingleton.implicitHeight * 4
}
