#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_File_Chooser.H>

#include <logic.hpp>

void onProjectRootDirInputChanged(Fl_Widget* widget, void* data)
{
    Logic* logic = static_cast<Logic*>(data);
    Fl_Input* input = static_cast<Fl_Input*>(widget);

    logic->trySetProjectRootDirectory(input->value());
}

void onFindInputChanged(Fl_Widget* widget, void* data)
{
    Logic* logic = static_cast<Logic*>(data);
    Fl_Input* input = static_cast<Fl_Input*>(widget);

	logic->tryGetOccurances(
		input->value()
	);
}

void onReplaceInputChanged(Fl_Widget* widget, void* data)
{
	std::cout << "Replace input changed: " << static_cast<Fl_Input*>(widget)->value() << std::endl;
	// You can add logic here to handle the change in the replace input.
}

void onMatchCaseChanged(Fl_Widget* widget, void* data)
{
	Logic* logic = static_cast<Logic*>(data);
	Fl_Check_Button* checkButton = static_cast<Fl_Check_Button*>(widget);

	logic->setMatchCase(checkButton->value());
}

void onWholeWordChanged(Fl_Widget* widget, void* data)
{
	Logic* logic = static_cast<Logic*>(data);
	Fl_Check_Button* checkButton = static_cast<Fl_Check_Button*>(widget);

	logic->setWholeWord(checkButton->value());
}

void onDirButtonClicked(Fl_Widget* widget, void* data)
{
	Logic* logic = static_cast<Logic*>(data);
	const char* dir = fl_dir_chooser("Choose Project Root Directory", nullptr, 0);
	if (dir)
	{
		logic->trySetProjectRootDirectory(dir);

		Fl_Input* input = static_cast<Fl_Input*>(widget->parent()->child(1));
		input->value(dir);
	}
}

int main(int argc, char **argv) {

	Logic logic;

	Fl_Window *window = new Fl_Window(340, 340);

	Fl_Box *projRootDirValidityBox = new Fl_Box(270, 18, 20, 20, "Invalid\nDirectory");
	
	Fl_Input* projRootDir = new Fl_Input(5, 5, 200, 20);
	projRootDir->when(FL_WHEN_CHANGED);
	projRootDir->callback(onProjectRootDirInputChanged, &logic);
	
	Fl_Input* findInput = new Fl_Input(5, 30, 200, 20);
	findInput->when(FL_WHEN_CHANGED);
	findInput->callback(onFindInputChanged, &logic);
	
	Fl_Input* replaceInput = new Fl_Input(5, 55, 200, 20);
	replaceInput->when(FL_WHEN_CHANGED);
	replaceInput->callback(onReplaceInputChanged, &logic);

	Fl_Button *dirButton = new Fl_Button(210, 5, 20, 20, "@fileopen");
	dirButton->callback(onDirButtonClicked, &logic);

	Fl_Button *findButton = new Fl_Button(210, 30, 20, 20, "@search");

	Fl_Check_Button *wholeButton = new Fl_Check_Button(5, 105, 20, 20, "Whole Word");
	wholeButton->callback(onWholeWordChanged, &logic);

	Fl_Check_Button *caseButton = new Fl_Check_Button(115, 105, 20, 20, "Match Case");
	caseButton->callback(onMatchCaseChanged, &logic);

	Fl_Button *nextButton = new Fl_Button(5, 80, 100, 20, "Replace Next");
	Fl_Button *allButton = new Fl_Button(110, 80, 80, 20, "Replace All");
	
	//
	Fl_Scroll *scroll = new Fl_Scroll(5,  130, 300, 200);
	scroll->box(FL_DOWN_BOX);
	scroll->begin();
	
	for (int i = 0; i < 20; ++i) {
		char label[32];
		sprintf(label, "Box %d", i + 1);
		new Fl_Box(5, 5 + (i * 20), 100, 20, strdup(label));
		//box->box(FL_UP_BOX); 
	}

	scroll->end();

	//
	window->end();
	//window->resizable(window);
	window->show(argc, argv);
	
	return Fl::run();

	return 0;
}
