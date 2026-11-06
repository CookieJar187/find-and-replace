#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>

#include <logic.hpp>

//
void updateScrollContent(Fl_Scroll* scroll, const std::vector<FileOccurrence>& occurrences)
{
	scroll->clear();
	scroll->begin();

	int yOffset = 15;
	for (const auto& occurrence : occurrences)
	{
		int groupHeight = 35 + occurrence.lineOccurrences.size() * 20;
		Fl_Group* group = new Fl_Group(10, yOffset, 290, groupHeight);
		group->box(FL_UP_BOX);
		group->align(FL_ALIGN_TOP_LEFT);
		group->clip_children(1);

		group->begin();
		
		Fl_Box* fileBox = new Fl_Box(10, yOffset, 290, 20);
		fileBox->copy_label(occurrence.filePath.filename().c_str());
		fileBox->box(FL_FLAT_BOX);
		fileBox->color(FL_LIGHT2);
		//fileBox->labelsize(14);
		fileBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		
		yOffset += 25;

		for (const auto& [lineNumber, lineContent] : occurrence.lineOccurrences)
		{
			std::string lineLabel = "Line " + std::to_string(lineNumber) + ": " + lineContent;
			Fl_Box* lineBox = new Fl_Box(25, yOffset, 290, 20);
			lineBox->copy_label(lineLabel.c_str());
			lineBox->box(FL_NO_BOX);
			lineBox->labelsize(12);
			lineBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
			yOffset += 20;
		}

		yOffset += 14; // Add some space between groups

		group->end();
	}

	scroll->end();
	//scroll->scroll_to(0, 0);
	scroll->redraw();
}

void tryFindOccurancesWrapper(Logic* logic, Fl_Scroll* scroll)
{
	logic->tryFindOccurances();
	updateScrollContent(scroll, logic->getOccurrences());
}

void onProjectRootDirInputChanged(Fl_Widget* widget, void* data)
{
    Logic* logic = static_cast<Logic*>(data);
    Fl_Input* projRootDir = static_cast<Fl_Input*>(widget);
	Fl_Scroll* scroll = static_cast<Fl_Scroll*>(widget->parent()->child(10));

    logic->trySetProjectRootDirectory(projRootDir->value());

	Fl_Box* projRootDirValidityBox = static_cast<Fl_Box*>(widget->parent()->child(0));

	if (logic->isValid())
	{
		projRootDirValidityBox->copy_label("Directory\nOK");
		projRootDirValidityBox->labelcolor(FL_BLACK);
	}
	else if (projRootDir->value()[0] == '\0')
		projRootDirValidityBox->copy_label("");
	else
	{
		projRootDirValidityBox->copy_label("Invalid\nDirectory");
		projRootDirValidityBox->labelcolor(FL_RED);
	}

	tryFindOccurancesWrapper(logic, scroll);
}

void onFindInputChanged(Fl_Widget* widget, void* data)
{
    Logic* logic = static_cast<Logic*>(data);
    Fl_Input* findInput = static_cast<Fl_Input*>(widget);
	Fl_Scroll* scroll = static_cast<Fl_Scroll*>(widget->parent()->child(10));

	logic->trySetSearchTerm(findInput->value());
	tryFindOccurancesWrapper(logic, scroll);
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

		Fl_Input* findInput = static_cast<Fl_Input*>(widget->parent()->child(1));
		findInput->value(dir);
	}
}

int main(int argc, char **argv) {

	Logic logic;

	Fl_Window *window = new Fl_Window(340, 340);

	Fl_Box *projRootDirValidityBox = new Fl_Box(245, 5, 70, 40);
	Fl_Input* projRootDir = new Fl_Input(5, 5, 200, 20);
	Fl_Input* findInput = new Fl_Input(5, 30, 200, 20);
	Fl_Input* replaceInput = new Fl_Input(5, 55, 200, 20);
	Fl_Button *dirButton = new Fl_Button(210, 5, 20, 20, "@fileopen");
	Fl_Button *findButton = new Fl_Button(210, 30, 20, 20, "@search");
	Fl_Check_Button *wholeButton = new Fl_Check_Button(5, 105, 20, 20, "Whole Word");
	Fl_Check_Button *caseButton = new Fl_Check_Button(115, 105, 20, 20, "Match Case");
	Fl_Button *nextButton = new Fl_Button(5, 80, 100, 20, "Replace Next");
	Fl_Button *allButton = new Fl_Button(110, 80, 80, 20, "Replace All");
	
	Fl_Scroll *scroll = new Fl_Scroll(5,  130, 300, 200);
	scroll->box(FL_DOWN_BOX);
	scroll->type(Fl_Scroll::VERTICAL_ALWAYS);
	//
	projRootDir->when(FL_WHEN_CHANGED);
	projRootDir->callback(onProjectRootDirInputChanged, &logic);

	findInput->when(FL_WHEN_CHANGED);
	findInput->callback(onFindInputChanged, &logic);

	replaceInput->when(FL_WHEN_CHANGED);
	replaceInput->callback(onReplaceInputChanged, &logic);

	dirButton->callback(onDirButtonClicked, &logic);

	wholeButton->callback(onWholeWordChanged, &logic);

	caseButton->callback(onMatchCaseChanged, &logic);

	//
	window->end();
	//window->resizable(window);
	window->show(argc, argv);
	
	return Fl::run();

	return 0;
}
