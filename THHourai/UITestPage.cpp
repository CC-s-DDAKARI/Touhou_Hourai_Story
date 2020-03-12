using namespace Touhou;

using namespace SC::Game::UI;
using namespace SC::Drawing;

UITestPage::UITestPage() : BasePage()
{
	InitializeComponents();
}

void UITestPage::InitializeComponents()
{
	var textFormat = new TextFormat("", 12);
	var textFormat2 = new TextFormat("", 10);

	declare_element( RelativePanel, relativePanel );
	{

		declare_element(FixedPanel, mypanel);
		mypanel->Width = 300;
		mypanel->Height = 30;
		relativePanel->Add(mypanel);
		
		declare_element(Shapes::Rectangle, myHPbar);
		myHPbar->Width = 0;
		myHPbar->Height = 0;
		myHPbar->Fill = new SolidColorBrush(Color::Red);
		mypanel->Add(myHPbar);
		
		declare_element(FixedPanel, mypanel2);
		mypanel2->Width = 300;
		mypanel2->Height = 30;
		relativePanel->Add(mypanel2);
		mypanel2->AddDependencyProperty("RelativePanel.Below=mypanel");

		declare_element(Shapes::Rectangle, myMPbar);
		myMPbar->Width = 0;
		myMPbar->Height = 0;
		myMPbar->Fill = new SolidColorBrush(Color::Blue);
		mypanel2->Add(myMPbar);

		declare_element(FixedPanel, mypanel3);
		mypanel3->Width = 300;
		mypanel3->Height = 10;
		relativePanel->Add(mypanel3);
		mypanel3->AddDependencyProperty("RelativePanel.Below=mypanel2");

		declare_element(Shapes::Rectangle, myEXPbar);
		myEXPbar->Width = 0;
		myEXPbar->Height = 0;
		myEXPbar->Fill = new SolidColorBrush(Color::Green);
		mypanel3->Add(myEXPbar);

		declare_element(TextBlock, myText);
		myText->Content = "<color=white>100%</color>";
		myText->Alignment = TextAlignment::Center;
		myText->VerticalAlignment = TextVerticalAlignment::Center;
		myText->AutoSizing = false;										//글자 자동 크기조절
		myText->Format = textFormat;
		mypanel->Add(myText);

		declare_element(TextBlock, myText2);
		myText2->Content = "<color=white>100%</color>";
		myText2->Alignment = TextAlignment::Center;
		myText2->VerticalAlignment = TextVerticalAlignment::Center;
		myText2->AutoSizing = false;										//글자 자동 크기조절
		myText2->Format = textFormat;
		mypanel2->Add(myText2);

		declare_element(TextBlock, myText3);
		myText3->Content = "<color=white>100%</color>";
		myText3->Alignment = TextAlignment::Center;
		myText3->VerticalAlignment = TextVerticalAlignment::Center;
		myText3->AutoSizing = false;										//글자 자동 크기조절
		myText3->Format = textFormat2;
		mypanel3->Add(myText3);

		/*
		declare_element( TextBlock, myText );
		myText->Content = "<color=Pink>dlwodudwnr</color>\ndlsekdlwodud";
		myText->Margin = 0;
		myText->Color = Color::Red;
		relativePanel->Add(myText);
			   
		declare_element( Border, myBorder );
		myBorder->Content = myText;
		myBorder->Fill = new RevealBrush( Color::White );
		myBorder->Padding = 10;
		relativePanel->Add( myBorder );
		
		declare_element( Button, myButton );
		myButton->Content = "MyButton";
		myButton->AddDependencyProperty( "RelativePanel.Below=myBorder" );
		relativePanel->Add( myButton );
				
		*/

	}

	Content = relativePanel;
}