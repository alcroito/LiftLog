import QtQuick 2.2

QtObject { 
	property var windowWidth: 320
	property var windowHeight: 480
	property var uncompletedWorkoutExists: false
	property var currentWorkoutModel: ListModel {
		ListElement { name: "Squat"; setsAndReps: "5x5"; weight: "20" }
		ListElement { name: "Bench Press"; setsAndReps: "4x8"; weight: "30" }
		ListElement { name: "Barbell Rows"; setsAndReps: "3x12"; weight: "40" }
	}
}