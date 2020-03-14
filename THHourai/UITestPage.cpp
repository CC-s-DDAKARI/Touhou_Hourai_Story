using namespace Touhou;

using namespace SC::Game::UI;
using namespace SC::Drawing;

UITestPage::UITestPage() : BasePage()
{
	InitializeComponents();
}

void UITestPage::InitializeComponents()
{
	declare_element( RelativePanel, relativePanel );
	{
		declare_element( TextBlock, myText );
		myText->Content = "MyText!";
		myText->Margin = 10;
		myText->Color = Color::Aquamarine;

		declare_element( Border, myBorder );
		myBorder->Content = myText;
		myBorder->Fill = new RevealBrush( Color::White );
		myBorder->Padding = 10;
		relativePanel->Add( myBorder );

		declare_element( Button, myButton );
		myButton->Content = "MyButton";
		myButton->AddDependencyProperty( "RelativePanel.Below=myBorder" );
		relativePanel->Add( myButton );

		declare_element( Shapes::Rectangle, myRect );
		myRect->Width = 300;
		myRect->Height = 300;
		myRect->Margin = Thickness( 500, 300 );
		myRect->Fill = new SolidColorBrush( Color::Pink );
		relativePanel->Add( myRect );

		declare_element( Shapes::Rectangle, myRect1 );
		myRect1->Width = 300;
		myRect1->Height = 100;
		myRect1->Fill = new SolidColorBrush( Color::LightGreen );
		myRect1->AddDependencyProperty( "RelativePanel.Below=myRect" );
		myRect1->AddDependencyProperty( "RelativePanel.AlignLeftWith=myRect" );
		relativePanel->Add( myRect1 );
	}

	Content = relativePanel;
}