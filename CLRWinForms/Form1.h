#pragma once

namespace CppCLRWinformsProjekt {
	ref class Form1;

	class Average
	{
	public:
		float comps[3];

		void Zero()
		{
			comps[0] = 0;
			comps[1] = 0;
			comps[2] = 0;
		}

		void Add(Average* avg)
		{
			this->comps[0] += avg->comps[0];
			this->comps[1] += avg->comps[1];
			this->comps[2] += avg->comps[2];
		}

		void Add(int x, int y, unsigned char* bmporg, int w, int h)
		{
			if (x < 0 || y < 0 || x >= w || y >= h)
			{
				//comps[0] += 255;
				//comps[1] += 255;
				//comps[2] += 255;
				return;
			}

			x += y * w;
			x *= 3;
			comps[0] += bmporg[x + 0];
			comps[1] += bmporg[x + 1];
			comps[2] += bmporg[x + 2];
		}

		void Sub(int x, int y, unsigned char* bmporg, int w, int h)
		{
			if (x < 0 || y < 0 || x >= w || y >= h)
			{
				//comps[0] -= 255;
				//comps[1] -= 255;
				//comps[2] -= 255;
				return;
			}

			x += y * w;
			x *= 3;
			comps[0] -= bmporg[x + 0];
			comps[1] -= bmporg[x + 1];
			comps[2] -= bmporg[x + 2];
		}

		void Multiply(float m)
		{
			comps[0] *= m;
			comps[1] *= m;
			comps[2] *= m;
		}

		void WritePixel(int a, int b, unsigned char* bmpDest, int w, int h)
		{
			if (a < 0 || b < 0 || a >= w || b >= h)
			{
				return;
			}
			a += b * w;
			a *= 3;

			bmpDest[a + 0] = (unsigned char)comps[0];
			bmpDest[a + 1] = (unsigned char)comps[1];
			bmpDest[a + 2] = (unsigned char)comps[2];
		}
	};
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Bitmap^ bmpFront;
		unsigned char* bmpOriginal;
		unsigned char* hBlur;
		unsigned char* keptIMG;

		static int imgSizeInBytes = -1;
		static Rectangle imgRect;
		BitmapData^ bmpData;

		static int saveCounter = 1;
		static String ^fileName;
		static String^ lastAction = "";

		static Color sColor, nColor;
		static bool keepLast = false;
		static bool applyNeg = false;

		static double Brightness_cppCount = 0.0;
		static double Brightness_cppTotal = 0.0;
		static double Brightness_asmCount = 0.0;
		static double Brightness_asmTotal = 0.0;
		static double Negative_asmCount = 0.0;
		static double Negative_asmTotal = 0.0;
		static double Negative_cppTotal = 0.0;
		static double Negative_cppCount = 0.0;
		static double Blur_asmTotal = 0.0;
		static double Blur_asmCount = 0.0;
		static double Blur_cppTotal = 0.0;
		static double Blur_cppCount = 0.0;
		static double ColorChange_cppTotal = 0.0;
		static double ColorChange_cppCount = 0.0;
		static double ColorChange_asmTotal = 0.0;
		static double ColorChange_asmCount = 0.0;
		static double Greyscale_asmCount = 0.0;
		static double Greyscale_asmTotal = 0.0;
		static double Greyscale_cppTotal = 0.0;
		static double Greyscale_cppCount = 0.0;

		void AdjustBrightness(unsigned char* bmp,unsigned char* org, short amount)
		{
			for (int i = 0; i < imgSizeInBytes; i++)
			{
				if ((short)org[i] + amount < 0) bmp[i] = 0;
				else if ((short)org[i] + amount > 255) bmp[i] = 255;
				else bmp[i] = org[i] + amount;
			}
		}

		void ResetIMG(unsigned char* bmp, unsigned char* original, long imgSize)
		{
			for (int i = 0; i < imgSize; i++)
			{
				bmp[i] = original[i];
			}
		}

		void CPPNegativeIMG(unsigned char* bmp, unsigned char* original,long imgSize,bool isChecked)
		{	
			if(isChecked)
			{
				for (int i = 0; i < imgSize; i++)
				{
					bmp[i] = 255 - original[i];
				}
			}
			else
			{
				for (int i = 0; i < imgSize; i++)
				{
					bmp[i] = original[i];
				}
			}
		}

		void CPPBlurIMG(unsigned char* bmp,unsigned char* org, short blurWidth)
		{
			int height, width;
			Average avg,tmp;
			float rcpboxWidth = (1.0f) / (blurWidth * 2 + 1);

			height = bmpFront->Height;
			width = bmpFront->Width;
			hBlur = new unsigned char [imgSizeInBytes];

			//Horizontal blur
			for(int y = 0; y<height ;y++)
			{
				avg.Zero();
				for(int x = -blurWidth-1;x<blurWidth;x++)
				{	
					avg.Add(x, y, org, width, height);
					
				}
				avg.Multiply(rcpboxWidth);

				for (int x = 0; x < width; x++)
				{
					tmp.Zero();
					tmp.Sub(x - blurWidth - 1, y, org, width, height);
					tmp.Add(x + blurWidth, y, org, width, height);

					tmp.Multiply(rcpboxWidth);

					avg.Add(&tmp);

					avg.WritePixel(x, y, hBlur, width, height);
				}

			}
			//Vertical blur
			for (int y = 0; y < width; y++)
			{
				avg.Zero();
				for (int x = -blurWidth - 1; x < blurWidth; x++)
				{
					avg.Add(y, x, hBlur, width, height);
				}

				avg.Multiply(rcpboxWidth);

				for (int x = 0; x < height; x++)
				{
					tmp.Zero();
					tmp.Sub(y, x - blurWidth - 1, hBlur, width, height);
					tmp.Add(y , x + blurWidth, hBlur, width, height);

					tmp.Multiply(rcpboxWidth);

					avg.Add(&tmp);

					avg.WritePixel(y, x, bmp, width, height);
				}

			}
		}

		void CPPColorChangeColorRange(unsigned char* bmp,unsigned char *org,Color searchColor, Color newColor, unsigned char rangeValue, int rangeType)
		{	
			// Search color
			unsigned char Sb = searchColor.B;
			unsigned char Sg = searchColor.G;
			unsigned char Sr = searchColor.R;

			// Destination color
			unsigned char Db = newColor.B;
			unsigned char Dg = newColor.G;
			unsigned char Dr = newColor.R;

			// Dimmer
			if (rangeType==0)
			{
				// Limits	
				unsigned char Lb = 0;
				unsigned char Lg = 0;
				unsigned char Lr = 0;

				// Underflow check
				if (Sb >= rangeValue) { Lb = Sb-rangeValue; }
				if (Sg >= rangeValue) { Lg = Sg-rangeValue; }
				if (Sr >= rangeValue) { Lr = Sr-rangeValue; }

				for (int p = 0; p < imgSizeInBytes; p+=3)
				{
					// Check red's range
					if (org[p] <= Sr && org[p] >= Lr)
					{	//Check green's range
						if (org[p+1] <= Sg && org[p+1] >= Lg)
						{	//Check blue's range
							if (org[p+2] <= Sb && org[p+2] >= Lb)
							{
								bmp[p] = Db;
								bmp[p + 1] = Dg;
								bmp[p + 2] = Dr;
							}
						}
					}
				}
			}
			// Brighter
			else if (rangeType == 1)
			{
				// Limits
				unsigned char Ub = 255;
				unsigned char Ug = 255;
				unsigned char Ur = 255;

				// Overflow check
				if (255-Sb >= rangeValue) { Ub = Sb + rangeValue; }
				if (255-Sg >= rangeValue) { Ug = Sg + rangeValue; }
				if (255-Sr >= rangeValue) { Ur = Sr + rangeValue; }

				for (int p = 0; p < imgSizeInBytes; p += 3)
				{
					// Check red's range
					if (org[p] >= Sr && org[p] <= Ur)
					{	//Check green's range
						if (org[p + 1] >= Sg && org[p + 1] <= Ug)
						{	//Check blue's range
							if (org[p + 2] >= Sb && org[p + 2] <= Ub)
							{
								bmp[p] = Db;
								bmp[p + 1] = Dg;
								bmp[p + 2] = Dr;
							}
						}
					}
				}
			}
			else if (rangeType == 2) 
			{	
				// Limits
				unsigned char Lb = 0;
				unsigned char Lg = 0;
				unsigned char Lr = 0;
				unsigned char Ub = 255;
				unsigned char Ug = 255;
				unsigned char Ur = 255;

				unsigned char upperRange, lowerRange;
				if ( rangeValue%2 )
				{
					upperRange = (unsigned char)rangeValue / 2;
					lowerRange = upperRange + 1;
				}
				else
				{
					upperRange = rangeValue / 2;
					lowerRange = upperRange;
				}
				// Underflow check
				if (Sb >= lowerRange) { Lb = Sb + lowerRange; }
				if (Sg >= lowerRange) { Lg = Sg + lowerRange; }
				if (Sr >= lowerRange) { Lr = Sr + lowerRange; }
				// Overflow check
				if (255 - Sb >= upperRange) { Ub = Sb + upperRange; }
				if (255 - Sg >= upperRange) { Ug = Sg + upperRange; }
				if (255 - Sr >= upperRange) { Ur = Sr + upperRange; }

				for (int p = 0; p < imgSizeInBytes; p += 3)
				{
					// Check red's range
					if ((org[p] <= Sr && org[p] >= Lr) || (org[p] >= Sr && org[p] <= Ur))
					{	//Check green's range
						if ((org[p+1] <= Sr && org[p+1] >= Lr) || (org[p+1] >= Sr && org[p+1] <= Ug))
						{	//Check blue's range
							if ((org[p+2] <= Sr && org[p+2] >= Lr) || (org[p+2] >= Sr && org[p+2] <= Ub))
							{
								bmp[p] = Db;
								bmp[p + 1] = Dg;
								bmp[p + 2] = Dr;
							}
						}
					}
				}
			}
		}

		void CPPGreyscaleIMG(unsigned char* bmp, unsigned char* original, long imgSize)
		{
			float total;
			unsigned char greyed;
			float org[3];
			for (int i = 0; i < imgSize; i+=3)
			{	
				org[0] = (float)original[i];
				org[1] = (float)original[i+1];
				org[2] = (float)original[i+2];
				total = org[0]+ org[1]+ org[2];

				if (total == 0)
				{
					continue;
				}
				greyed = (unsigned char)( ((org[0] / total) * org[0]) + ((org[1] / total) * org[1]) + ((org[2] / total) * org[2]));
				bmp[i] = greyed;
				bmp[i+1] = greyed;
				bmp[i+2] = greyed;
			}
			
		}

		void ClearOriginalImage()
		{
			if (bmpOriginal != nullptr) { delete[] bmpOriginal; }
		}

		//Make a copy of the original image
		void SaveOriginalImage(System::Drawing::Bitmap^ bmp)
		{
			ClearOriginalImage();

			imgSizeInBytes = bmp->Width * bmp->Height * 3;

			bmpOriginal = new unsigned char[imgSizeInBytes];

			imgRect.Width = bmp->Width;
			imgRect.Height = bmp->Height;

			bmpData = bmp->LockBits(imgRect, ImageLockMode::ReadOnly, PixelFormat::Format24bppRgb);

			unsigned char* p = (unsigned char*)bmpData->Scan0.ToPointer();

			for (int i = 0; i < imgSizeInBytes; i++)
			{
				bmpOriginal[i] = *p++;
			}

			bmp->UnlockBits(bmpData);

		}

		void Copy(unsigned char* bmp,unsigned char* dest) 
		{
			for (int i = 0; i < imgSizeInBytes; i++) {
				dest[i] = (char)bmp[i];
			}
		}

		Form1(void)
		{
			InitializeComponent();

		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;

	private: System::Windows::Forms::PictureBox^ pictureBoxImg;
	private: System::Windows::Forms::Label^ averageTimeLabel;

	private: System::Windows::Forms::OpenFileDialog^ dlgOpen;

	private: System::Windows::Forms::TrackBar^ brightnessTrackbar;
	private: System::Windows::Forms::Label^ brightnessLabel;

	private: System::Windows::Forms::Label^ optionsLabel;


	private: System::Windows::Forms::TrackBar^ blurTrackbar;
	private: System::Windows::Forms::Label^ blurLabel;

	private: System::Windows::Forms::CheckBox^ ASMCheckBox;
	private: System::Windows::Forms::TrackBar^ ColorRangeScroll;


	private: System::Windows::Forms::Label^ ColorRangeLabel;

	private: System::Windows::Forms::Label^ RangeTypeLabel;
	private: System::Windows::Forms::ComboBox^ rangeDropDown;
	private: System::Windows::Forms::Label^ searchColorLabel;
	private: System::Windows::Forms::Label^ newColorLabel;
	private: System::Windows::Forms::ColorDialog^ SearchColorDiag;
	private: System::Windows::Forms::ColorDialog^ newColorDiag;
	private: System::Windows::Forms::TextBox^ searchColorText;
	private: System::Windows::Forms::TextBox^ newColorText;
	private: System::Windows::Forms::Button^ searchColorButton;
	private: System::Windows::Forms::Button^ newColorButton;
	private: System::Windows::Forms::ColorDialog^ colorChangeDialog;
	private: System::Windows::Forms::Button^ colorApplyButton;

	private: System::Windows::Forms::Button^ resetButton;
	private: System::Windows::Forms::CheckBox^ keepIMGCheckBox;
	private: System::Windows::Forms::ToolStripMenuItem^ saveMenuItem;
	private: System::Windows::Forms::SaveFileDialog^ saveFileDialog;
	private: System::ComponentModel::Container^ components;

	private: System::Windows::Forms::Button^ greyscaleButton;
	private: System::Windows::Forms::Button^ negativeButton;

	#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Erforderliche Methode für die Designerunterstützung.
		   /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			   this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->saveMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->pictureBoxImg = (gcnew System::Windows::Forms::PictureBox());
			   this->averageTimeLabel = (gcnew System::Windows::Forms::Label());
			   this->dlgOpen = (gcnew System::Windows::Forms::OpenFileDialog());
			   this->brightnessTrackbar = (gcnew System::Windows::Forms::TrackBar());
			   this->brightnessLabel = (gcnew System::Windows::Forms::Label());
			   this->optionsLabel = (gcnew System::Windows::Forms::Label());
			   this->blurTrackbar = (gcnew System::Windows::Forms::TrackBar());
			   this->blurLabel = (gcnew System::Windows::Forms::Label());
			   this->ASMCheckBox = (gcnew System::Windows::Forms::CheckBox());
			   this->ColorRangeScroll = (gcnew System::Windows::Forms::TrackBar());
			   this->ColorRangeLabel = (gcnew System::Windows::Forms::Label());
			   this->RangeTypeLabel = (gcnew System::Windows::Forms::Label());
			   this->rangeDropDown = (gcnew System::Windows::Forms::ComboBox());
			   this->searchColorLabel = (gcnew System::Windows::Forms::Label());
			   this->newColorLabel = (gcnew System::Windows::Forms::Label());
			   this->searchColorText = (gcnew System::Windows::Forms::TextBox());
			   this->newColorText = (gcnew System::Windows::Forms::TextBox());
			   this->searchColorButton = (gcnew System::Windows::Forms::Button());
			   this->newColorButton = (gcnew System::Windows::Forms::Button());
			   this->colorChangeDialog = (gcnew System::Windows::Forms::ColorDialog());
			   this->colorApplyButton = (gcnew System::Windows::Forms::Button());
			   this->resetButton = (gcnew System::Windows::Forms::Button());
			   this->keepIMGCheckBox = (gcnew System::Windows::Forms::CheckBox());
			   this->saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			   this->greyscaleButton = (gcnew System::Windows::Forms::Button());
			   this->negativeButton = (gcnew System::Windows::Forms::Button());
			   this->menuStrip1->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->brightnessTrackbar))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurTrackbar))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ColorRangeScroll))->BeginInit();
			   this->SuspendLayout();
			   this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			   this->menuStrip1->Location = System::Drawing::Point(0, 0);
			   this->menuStrip1->Name = L"menuStrip1";
			   this->menuStrip1->Size = System::Drawing::Size(883, 24);
			   this->menuStrip1->TabIndex = 0;
			   this->menuStrip1->Text = L"menuStrip1";
			   this->menuStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &Form1::menuStrip1_ItemClicked);
			   this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				   this->openToolStripMenuItem,
					   this->saveMenuItem, this->exitToolStripMenuItem
			   });
			   this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			   this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			   this->fileToolStripMenuItem->Text = L"File";
			   this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			   this->openToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			   this->openToolStripMenuItem->Text = L"&Open";
			   this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			   this->saveMenuItem->Enabled = false;
			   this->saveMenuItem->Name = L"saveMenuItem";
			   this->saveMenuItem->Size = System::Drawing::Size(103, 22);
			   this->saveMenuItem->Text = L"&Save";
			   this->saveMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveMenuItem_Click);
			   this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			   this->exitToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			   this->exitToolStripMenuItem->Text = L"&Exit";
			   this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			   this->pictureBoxImg->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->pictureBoxImg->BackColor = System::Drawing::SystemColors::Control;
			   this->pictureBoxImg->Location = System::Drawing::Point(13, 27);
			   this->pictureBoxImg->Name = L"pictureBoxImg";
			   this->pictureBoxImg->Size = System::Drawing::Size(859, 395);
			   this->pictureBoxImg->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBoxImg->TabIndex = 1;
			   this->pictureBoxImg->TabStop = false;
			   this->averageTimeLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->averageTimeLabel->AutoSize = true;
			   this->averageTimeLabel->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			   this->averageTimeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->averageTimeLabel->Location = System::Drawing::Point(13, 646);
			   this->averageTimeLabel->Margin = System::Windows::Forms::Padding(0, 0, 3, 0);
			   this->averageTimeLabel->Name = L"averageTimeLabel";
			   this->averageTimeLabel->Size = System::Drawing::Size(120, 20);
			   this->averageTimeLabel->TabIndex = 3;
			   this->averageTimeLabel->Text = L"Average (ms) : -";
			   this->averageTimeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			   this->averageTimeLabel->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			   this->dlgOpen->FileName = L"openFileDialog1";
			   this->dlgOpen->Filter = L"JPEG|*.jpg|PNG|*.png|Bitmap|*.bmp|All Files|*.*";
			   this->brightnessTrackbar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->brightnessTrackbar->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->brightnessTrackbar->Enabled = false;
			   this->brightnessTrackbar->Location = System::Drawing::Point(12, 456);
			   this->brightnessTrackbar->Maximum = 255;
			   this->brightnessTrackbar->Minimum = -255;
			   this->brightnessTrackbar->Name = L"brightnessTrackbar";
			   this->brightnessTrackbar->Size = System::Drawing::Size(206, 45);
			   this->brightnessTrackbar->TabIndex = 4;
			   this->brightnessTrackbar->TickFrequency = 16;
			   this->brightnessTrackbar->Scroll += gcnew System::EventHandler(this, &Form1::brightnessTrackbar_Scroll);
			   this->brightnessLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->brightnessLabel->BackColor = System::Drawing::SystemColors::MenuHighlight;
			   this->brightnessLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->brightnessLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->brightnessLabel->ForeColor = System::Drawing::Color::White;
			   this->brightnessLabel->Location = System::Drawing::Point(12, 430);
			   this->brightnessLabel->Name = L"brightnessLabel";
			   this->brightnessLabel->Size = System::Drawing::Size(206, 26);
			   this->brightnessLabel->TabIndex = 10;
			   this->brightnessLabel->Text = L"Brightness";
			   this->brightnessLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->optionsLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->optionsLabel->BackColor = System::Drawing::SystemColors::MenuHighlight;
			   this->optionsLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->optionsLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->optionsLabel->ForeColor = System::Drawing::Color::White;
			   this->optionsLabel->Location = System::Drawing::Point(236, 430);
			   this->optionsLabel->Name = L"optionsLabel";
			   this->optionsLabel->Size = System::Drawing::Size(143, 26);
			   this->optionsLabel->TabIndex = 11;
			   this->optionsLabel->Text = L"Options";
			   this->optionsLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->blurTrackbar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->blurTrackbar->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->blurTrackbar->Enabled = false;
			   this->blurTrackbar->LargeChange = 10;
			   this->blurTrackbar->Location = System::Drawing::Point(12, 540);
			   this->blurTrackbar->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
			   this->blurTrackbar->Maximum = 100;
			   this->blurTrackbar->Name = L"blurTrackbar";
			   this->blurTrackbar->Size = System::Drawing::Size(206, 45);
			   this->blurTrackbar->TabIndex = 1;
			   this->blurTrackbar->TickFrequency = 10;
			   this->blurTrackbar->Scroll += gcnew System::EventHandler(this, &Form1::blurTrackbar_Scroll);
			   this->blurLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->blurLabel->BackColor = System::Drawing::SystemColors::MenuHighlight;
			   this->blurLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->blurLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->blurLabel->ForeColor = System::Drawing::Color::White;
			   this->blurLabel->Location = System::Drawing::Point(12, 514);
			   this->blurLabel->Name = L"blurLabel";
			   this->blurLabel->Size = System::Drawing::Size(206, 26);
			   this->blurLabel->TabIndex = 14;
			   this->blurLabel->Text = L"Blur Width";
			   this->blurLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->ASMCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->ASMCheckBox->AutoSize = true;
			   this->ASMCheckBox->BackColor = System::Drawing::Color::Red;
			   this->ASMCheckBox->Enabled = false;
			   this->ASMCheckBox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->ASMCheckBox->FlatAppearance->CheckedBackColor = System::Drawing::Color::Lime;
			   this->ASMCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->ASMCheckBox->ForeColor = System::Drawing::Color::White;
			   this->ASMCheckBox->Location = System::Drawing::Point(694, 647);
			   this->ASMCheckBox->Name = L"ASMCheckBox";
			   this->ASMCheckBox->Size = System::Drawing::Size(78, 17);
			   this->ASMCheckBox->TabIndex = 15;
			   this->ASMCheckBox->Text = L"Use ASM";
			   this->ASMCheckBox->UseVisualStyleBackColor = false;
			   this->ColorRangeScroll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->ColorRangeScroll->BackColor = System::Drawing::SystemColors::WindowFrame;
			   this->ColorRangeScroll->Enabled = false;
			   this->ColorRangeScroll->Location = System::Drawing::Point(697, 522);
			   this->ColorRangeScroll->Maximum = 255;
			   this->ColorRangeScroll->Name = L"ColorRangeScroll";
			   this->ColorRangeScroll->Size = System::Drawing::Size(176, 45);
			   this->ColorRangeScroll->TabIndex = 1;
			   this->ColorRangeScroll->TickFrequency = 10;
			   this->ColorRangeScroll->Scroll += gcnew System::EventHandler(this, &Form1::ColorRangeScroll_Scroll);
			   this->ColorRangeLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->ColorRangeLabel->BackColor = System::Drawing::SystemColors::ActiveCaption;
			   this->ColorRangeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->ColorRangeLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->ColorRangeLabel->Location = System::Drawing::Point(697, 503);
			   this->ColorRangeLabel->Name = L"ColorRangeLabel";
			   this->ColorRangeLabel->Size = System::Drawing::Size(175, 18);
			   this->ColorRangeLabel->TabIndex = 16;
			   this->ColorRangeLabel->Text = L"Color Range";
			   this->ColorRangeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->RangeTypeLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->RangeTypeLabel->BackColor = System::Drawing::SystemColors::ActiveCaption;
			   this->RangeTypeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->RangeTypeLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->RangeTypeLabel->Location = System::Drawing::Point(697, 569);
			   this->RangeTypeLabel->Name = L"RangeTypeLabel";
			   this->RangeTypeLabel->Size = System::Drawing::Size(96, 21);
			   this->RangeTypeLabel->TabIndex = 17;
			   this->RangeTypeLabel->Text = L"Range Type";
			   this->RangeTypeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->rangeDropDown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->rangeDropDown->Enabled = false;
			   this->rangeDropDown->FormattingEnabled = true;
			   this->rangeDropDown->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Dimmer", L"Brighter", L"Centered" });
			   this->rangeDropDown->Location = System::Drawing::Point(812, 570);
			   this->rangeDropDown->Name = L"rangeDropDown";
			   this->rangeDropDown->Size = System::Drawing::Size(61, 21);
			   this->rangeDropDown->TabIndex = 18;
			   this->rangeDropDown->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::rangeDropDown_SelectedIndexChanged);
			   this->searchColorLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->searchColorLabel->BackColor = System::Drawing::SystemColors::Highlight;
			   this->searchColorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->searchColorLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->searchColorLabel->Location = System::Drawing::Point(697, 430);
			   this->searchColorLabel->Name = L"searchColorLabel";
			   this->searchColorLabel->Size = System::Drawing::Size(70, 15);
			   this->searchColorLabel->TabIndex = 19;
			   this->searchColorLabel->Text = L"Search Color";
			   this->searchColorLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->newColorLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->newColorLabel->BackColor = System::Drawing::SystemColors::Highlight;
			   this->newColorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->newColorLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->newColorLabel->Location = System::Drawing::Point(796, 430);
			   this->newColorLabel->Name = L"newColorLabel";
			   this->newColorLabel->Size = System::Drawing::Size(74, 15);
			   this->newColorLabel->TabIndex = 20;
			   this->newColorLabel->Text = L"New Color";
			   this->newColorLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->searchColorText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->searchColorText->Enabled = false;
			   this->searchColorText->Location = System::Drawing::Point(697, 450);
			   this->searchColorText->Name = L"searchColorText";
			   this->searchColorText->Size = System::Drawing::Size(70, 20);
			   this->searchColorText->TabIndex = 21;
			   this->newColorText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->newColorText->Enabled = false;
			   this->newColorText->Location = System::Drawing::Point(796, 450);
			   this->newColorText->Name = L"newColorText";
			   this->newColorText->Size = System::Drawing::Size(75, 20);
			   this->newColorText->TabIndex = 22;
			   this->searchColorButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->searchColorButton->CausesValidation = false;
			   this->searchColorButton->Enabled = false;
			   this->searchColorButton->Location = System::Drawing::Point(697, 476);
			   this->searchColorButton->Name = L"searchColorButton";
			   this->searchColorButton->Size = System::Drawing::Size(70, 24);
			   this->searchColorButton->TabIndex = 23;
			   this->searchColorButton->Text = L"Pick";
			   this->searchColorButton->UseVisualStyleBackColor = true;
			   this->searchColorButton->Click += gcnew System::EventHandler(this, &Form1::searchColorButton_Click);
			   this->newColorButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->newColorButton->Enabled = false;
			   this->newColorButton->Location = System::Drawing::Point(796, 476);
			   this->newColorButton->Name = L"newColorButton";
			   this->newColorButton->Size = System::Drawing::Size(74, 24);
			   this->newColorButton->TabIndex = 24;
			   this->newColorButton->Text = L"Pick";
			   this->newColorButton->UseVisualStyleBackColor = true;
			   this->newColorButton->Click += gcnew System::EventHandler(this, &Form1::newColorButton_Click);
			   this->colorChangeDialog->AnyColor = true;
			   this->colorChangeDialog->FullOpen = true;
			   this->colorChangeDialog->SolidColorOnly = true;
			   this->colorApplyButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->colorApplyButton->CausesValidation = false;
			   this->colorApplyButton->Enabled = false;
			   this->colorApplyButton->Location = System::Drawing::Point(694, 594);
			   this->colorApplyButton->Name = L"colorApplyButton";
			   this->colorApplyButton->Size = System::Drawing::Size(179, 24);
			   this->colorApplyButton->TabIndex = 25;
			   this->colorApplyButton->Text = L"Change Colors";
			   this->colorApplyButton->UseVisualStyleBackColor = true;
			   this->colorApplyButton->Click += gcnew System::EventHandler(this, &Form1::colorApplyButton_Click);
			   this->resetButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->resetButton->BackColor = System::Drawing::Color::Red;
			   this->resetButton->Enabled = false;
			   this->resetButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->resetButton->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->resetButton->Location = System::Drawing::Point(810, 647);
			   this->resetButton->Name = L"resetButton";
			   this->resetButton->Size = System::Drawing::Size(60, 21);
			   this->resetButton->TabIndex = 26;
			   this->resetButton->Text = L"RESET";
			   this->resetButton->UseVisualStyleBackColor = false;
			   this->resetButton->Click += gcnew System::EventHandler(this, &Form1::resetButton_Click);
			   this->keepIMGCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->keepIMGCheckBox->AutoSize = true;
			   this->keepIMGCheckBox->BackColor = System::Drawing::Color::Red;
			   this->keepIMGCheckBox->Enabled = false;
			   this->keepIMGCheckBox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->keepIMGCheckBox->FlatAppearance->CheckedBackColor = System::Drawing::Color::Lime;
			   this->keepIMGCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->keepIMGCheckBox->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			   this->keepIMGCheckBox->Location = System::Drawing::Point(236, 522);
			   this->keepIMGCheckBox->Name = L"keepIMGCheckBox";
			   this->keepIMGCheckBox->Size = System::Drawing::Size(116, 17);
			   this->keepIMGCheckBox->TabIndex = 27;
			   this->keepIMGCheckBox->Text = L"Apply to current";
			   this->keepIMGCheckBox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->keepIMGCheckBox->UseVisualStyleBackColor = false;
			   this->keepIMGCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::keepIMGCheckBox_CheckedChanged);
			   this->saveFileDialog->Filter = L"JPEG(*.jpg)|*.jpg|PNG(*.png)|*.png|Bitmap(*.bmp)|*.bmp";
			   this->greyscaleButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->greyscaleButton->CausesValidation = false;
			   this->greyscaleButton->Enabled = false;
			   this->greyscaleButton->Location = System::Drawing::Point(236, 489);
			   this->greyscaleButton->Name = L"greyscaleButton";
			   this->greyscaleButton->Size = System::Drawing::Size(69, 24);
			   this->greyscaleButton->TabIndex = 28;
			   this->greyscaleButton->Text = L"Greyscale";
			   this->greyscaleButton->UseVisualStyleBackColor = true;
			   this->greyscaleButton->Click += gcnew System::EventHandler(this, &Form1::greyscaleButton_Click);
			   this->negativeButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->negativeButton->CausesValidation = false;
			   this->negativeButton->Enabled = false;
			   this->negativeButton->Location = System::Drawing::Point(236, 459);
			   this->negativeButton->Name = L"negativeButton";
			   this->negativeButton->Size = System::Drawing::Size(69, 24);
			   this->negativeButton->TabIndex = 29;
			   this->negativeButton->Text = L"Negative";
			   this->negativeButton->UseVisualStyleBackColor = true;
			   this->negativeButton->Click += gcnew System::EventHandler(this, &Form1::negativeButton_Click);
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			   this->BackColor = System::Drawing::SystemColors::ButtonShadow;
			   this->ClientSize = System::Drawing::Size(883, 675);
			   this->Controls->Add(this->negativeButton);
			   this->Controls->Add(this->greyscaleButton);
			   this->Controls->Add(this->keepIMGCheckBox);
			   this->Controls->Add(this->resetButton);
			   this->Controls->Add(this->colorApplyButton);
			   this->Controls->Add(this->newColorButton);
			   this->Controls->Add(this->searchColorButton);
			   this->Controls->Add(this->newColorText);
			   this->Controls->Add(this->searchColorText);
			   this->Controls->Add(this->newColorLabel);
			   this->Controls->Add(this->searchColorLabel);
			   this->Controls->Add(this->rangeDropDown);
			   this->Controls->Add(this->RangeTypeLabel);
			   this->Controls->Add(this->ColorRangeLabel);
			   this->Controls->Add(this->ColorRangeScroll);
			   this->Controls->Add(this->ASMCheckBox);
			   this->Controls->Add(this->blurLabel);
			   this->Controls->Add(this->blurTrackbar);
			   this->Controls->Add(this->optionsLabel);
			   this->Controls->Add(this->brightnessLabel);
			   this->Controls->Add(this->brightnessTrackbar);
			   this->Controls->Add(this->averageTimeLabel);
			   this->Controls->Add(this->pictureBoxImg);
			   this->Controls->Add(this->menuStrip1);
			   this->MainMenuStrip = this->menuStrip1;
			   this->Name = L"Form1";
			   this->Text = L"IMG Proc";
			   this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			   this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			   this->menuStrip1->ResumeLayout(false);
			   this->menuStrip1->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->brightnessTrackbar))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurTrackbar))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ColorRangeScroll))->EndInit();
			   this->ResumeLayout(false);
			   this->PerformLayout();

		   }
	#pragma endregion
		private: System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
		{
			Application::Exit();
		}
		private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void menuStrip1_ItemClicked(System::Object^ sender, System::Windows::Forms::ToolStripItemClickedEventArgs^ e) {
		}
		private: System::Void openToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (dlgOpen->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				try
				{	
					bmpFront = (Bitmap^)Image::FromFile(dlgOpen->FileName);
					if (!bmpFront->PixelFormat.Equals(PixelFormat::Format24bppRgb) )
					{	
						MessageBox::Show("Image's bit depth should be 24, not " + bmpFront->PixelFormat.ToString());
						return;
					}
					else 
					{
						SaveOriginalImage(bmpFront);
					}
					
					String^ fullname = dlgOpen->SafeFileName;
					int name_length = fullname->Length;
					int i = name_length;
					wchar_t dot = (wchar_t)'.';

					// Store index of the extension starting
					for (; i > 0; i--) {if (fullname[i - 1] == dot) {break;}}
					// Write characters until extension starts
					for (int j = 0; j < i-1; j++) {fileName += fullname[j];}

					pictureBoxImg->Image = bmpFront;
					
					ASMCheckBox->Enabled = true;
					brightnessTrackbar->Enabled = true;
					blurTrackbar->Enabled = true;
					ColorRangeScroll->Enabled = true;
					resetButton->Enabled = true;
					searchColorButton->Enabled = true;
					searchColorText->Enabled = true;
					newColorButton->Enabled = true;
					newColorText->Enabled = true;
					rangeDropDown->Enabled = true;
					keepIMGCheckBox->Enabled = true;
					saveMenuItem->Enabled = true;
					negativeButton->Enabled = true;
					greyscaleButton->Enabled = true;
					rangeDropDown->Enabled = true;

					Brightness_cppTotal = 0.0;
					Brightness_cppCount = 0.0;
					Brightness_asmCount = 0.0;
					Brightness_asmTotal = 0.0;
					Blur_cppTotal = 0.0;
					Blur_cppCount = 0.0;
					Blur_asmCount = 0.0;
					Blur_asmTotal = 0.0;
					Negative_asmCount = 0.0;
					Negative_asmTotal = 0.0;
					Negative_cppCount = 0.0;
					Negative_cppTotal = 0.0;
					Greyscale_asmCount = 0.0;
					Greyscale_asmTotal = 0.0;
					Greyscale_cppCount = 0.0;
					Greyscale_cppTotal = 0.0;
					ColorChange_asmCount = 0.0;
					ColorChange_asmTotal = 0.0;
					ColorChange_cppCount = 0.0;
					ColorChange_cppTotal = 0.0;

					keptIMG = new unsigned char[imgSizeInBytes];
				}
				catch (Exception ^err)
				{
					MessageBox::Show("File could not be opened!\n" + err->Message);
				}
			}
		}
		private: System::Void brightnessTrackbar_Scroll(System::Object^ sender, System::EventArgs^ e)
		{	
			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;
			brightnessLabel->Text = "Brightness: " + brightnessTrackbar->Value.ToString();

			if (keepLast) { Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG); }
			else { Copy(bmpOriginal, keptIMG); }

			if (ASMCheckBox->Checked)
			{
				startTime = clock();
				ASMAdjustBrightness((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, brightnessTrackbar->Value, imgSizeInBytes);
				finishTime = clock();
				Brightness_asmTotal += finishTime - startTime;
				Brightness_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(Brightness_asmTotal / Brightness_asmCount, 2) + "ms";
			}
			else
			{
				startTime = clock();
				AdjustBrightness((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, brightnessTrackbar->Value);
				finishTime = clock();
				Brightness_cppTotal += finishTime - startTime;
				Brightness_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(Brightness_cppTotal / Brightness_cppCount, 2) + "ms";
			}
			bmpFront->UnlockBits(bmpData);

			pictureBoxImg->Image = bmpFront;

			lastAction = "_brightness_changed";
		}

		private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
		{
			ClearOriginalImage();
		}

		private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e)
		{

		}

		private: System::Void blurTrackbar_Scroll(System::Object^ sender, System::EventArgs^ e) 
		{	
			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;
			int blurWidth = blurTrackbar->Value;
			blurLabel->Text = "Blur Width: " + blurTrackbar->Value.ToString();

			if (keepLast) { Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG); }
			else { Copy(bmpOriginal, keptIMG); }

			if (ASMCheckBox->Checked)
			{
				hBlur = new unsigned char[imgSizeInBytes];
				startTime = clock();
				ASMBlurIMG(bmpFront->Height,bmpFront->Width, keptIMG, (unsigned char*)bmpData->Scan0.ToPointer(),hBlur,blurWidth);
				finishTime = clock();
				Blur_asmTotal += finishTime - startTime;
				Blur_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(Blur_asmTotal / Blur_asmCount, 2) + "ms";
			}
			else
			{
				startTime = clock();
				CPPBlurIMG((unsigned char*)bmpData->Scan0.ToPointer(),keptIMG, blurWidth);
				finishTime = clock();
				Blur_cppTotal += finishTime - startTime;
				Blur_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(Blur_cppTotal / Blur_cppCount, 2) + "ms";
				
			}
			bmpFront->UnlockBits(bmpData);
			pictureBoxImg->Image = bmpFront;
			delete hBlur;

			lastAction = "_blurred";
		}

		private: System::Void searchColorButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			colorChangeDialog->ShowDialog();
			sColor = colorChangeDialog->Color;
			searchColorText->Text = sColor.Name;
			
		}
		private: System::Void newColorButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			colorChangeDialog->ShowDialog();
			nColor = colorChangeDialog->Color;
			newColorText->Text = nColor.Name;
		}
		private: System::Void ColorRangeScroll_Scroll(System::Object^ sender, System::EventArgs^ e) 
		{
			ColorRangeLabel->Text = "Color Range: " + ColorRangeScroll->Value.ToString();
		}
		private: System::Void rangeDropDown_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			if (rangeDropDown->SelectedIndex == -1) { ColorRangeScroll->Enabled = false; }
			else { colorApplyButton->Enabled = true; }
		}
		private: System::Void resetButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			ResetIMG((unsigned char*)bmpData->Scan0.ToPointer(), bmpOriginal, imgSizeInBytes);
			bmpFront->UnlockBits(bmpData);

			pictureBoxImg->Image = bmpFront;
			averageTimeLabel->Text = "Average (ms) :-";

			brightnessTrackbar->Value = 0;
			brightnessLabel->Text = "Brightness";
			
			blurTrackbar->Value = 0;
			blurLabel->Text = "Blur Width";

			ColorRangeScroll->Value = 0;
			ColorRangeLabel->Text = "Color Range";
			
			rangeDropDown->ResetText();
			
			searchColorText->ResetText();
			newColorText->ResetText();
			
			ASMCheckBox->Checked = false;
			keepIMGCheckBox->Checked = false;
			applyNeg = false;

			Brightness_cppTotal = 0.0;
			Brightness_cppCount = 0.0;
			Brightness_asmCount = 0.0;
			Brightness_asmTotal = 0.0;
			Blur_cppTotal = 0.0;
			Blur_cppCount = 0.0;
			Blur_asmCount = 0.0;
			Blur_asmTotal = 0.0;
			Negative_asmCount = 0.0;
			Negative_asmTotal = 0.0;
			Negative_cppCount = 0.0;
			Negative_cppTotal = 0.0;
			Greyscale_asmCount = 0.0;
			Greyscale_asmTotal = 0.0;
			Greyscale_cppCount = 0.0;
			Greyscale_cppTotal = 0.0;
			ColorChange_asmCount = 0.0;
			ColorChange_asmTotal = 0.0;
			ColorChange_cppCount = 0.0;
			ColorChange_cppTotal = 0.0;

			lastAction = "";

			keptIMG = new unsigned char[imgSizeInBytes];
		}
		private: System::Void colorApplyButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			if (searchColorText->Text->Equals(String::Empty) || newColorText->Text->Equals(String::Empty))
			{
				return;
			}

			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;
			unsigned char brightnessValue = ColorRangeScroll->Value;

			if (keepLast) { Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG);
			}
			else { 
				Copy(bmpOriginal, keptIMG);
				ResetIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes);
			}

			if (ASMCheckBox->Checked)
			{
				startTime = clock();
				//ASMBlurIMG(bmpFront->Height, bmpFront->Width, bmpOriginal, (unsigned char*)bmpData->Scan0.ToPointer(), hBlur, blurWidth);
				finishTime = clock();
				ColorChange_asmTotal += finishTime - startTime;
				ColorChange_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(ColorChange_asmTotal / ColorChange_asmCount, 2) + "ms";
			}
			else
			{
				startTime = clock();
				CPPColorChangeColorRange((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, sColor, nColor, brightnessValue, rangeDropDown->SelectedIndex);
				finishTime = clock();
				ColorChange_cppTotal += finishTime - startTime;
				ColorChange_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(ColorChange_cppTotal / ColorChange_cppCount, 2) + "ms";

			}
			bmpFront->UnlockBits(bmpData);
			pictureBoxImg->Image = bmpFront;
			lastAction = "_color_range_changed";
		}
		
		private: System::Void keepIMGCheckBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			keepLast = keepIMGCheckBox->Checked;
		}
		private: System::Void saveMenuItem_Click(System::Object^ sender, System::EventArgs^ e) 
		{	
			saveFileDialog->FileName = fileName + lastAction + "_" + saveCounter + "_" + DateTime::Now.ToString("yyyyMMdd-hh_mm_ss");
			if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				try
				{	
					bmpFront->Save(saveFileDialog->FileName);
					saveCounter++;
				}
				catch (...)
				{
					MessageBox::Show("File could not be saved!");
				}
			}
		}
		private: System::Void negativeButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{	
			applyNeg = !applyNeg;

			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;

			if (keepLast) { Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG); applyNeg = true; }
			else { Copy(bmpOriginal, keptIMG); }

			if (ASMCheckBox->Checked)
			{
				startTime = clock();
				ASMNegativeIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes, applyNeg);
				finishTime = clock();
				Negative_asmTotal += finishTime - startTime;
				Negative_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(Negative_asmTotal / Negative_asmCount, 2) + "ms";
			}
			else
			{
				startTime = clock();
				CPPNegativeIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes, applyNeg);
				finishTime = clock();
				Negative_cppTotal += finishTime - startTime;
				Negative_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(Negative_cppTotal / Negative_cppCount, 2) + "ms";
			}
			bmpFront->UnlockBits(bmpData);

			pictureBoxImg->Image = bmpFront;

			lastAction = "_negated";
		}
		private: System::Void greyscaleButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;

			if (keepLast) { Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG); }
			else { Copy(bmpOriginal, keptIMG); }

			if (ASMCheckBox->Checked)
			{
				startTime = clock();
				//ASMNegativeIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes, applyNeg);
				finishTime = clock();
				Negative_asmTotal += finishTime - startTime;
				Negative_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(Negative_asmTotal / Negative_asmCount, 2) + "ms";
			}
			else
			{
				startTime = clock();
				CPPGreyscaleIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes);
				finishTime = clock();
				Greyscale_cppTotal += finishTime - startTime;
				Greyscale_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(Greyscale_cppTotal / Greyscale_cppCount, 2) + "ms";
			}
			bmpFront->UnlockBits(bmpData);

			pictureBoxImg->Image = bmpFront;

			lastAction = "_greyscaled";
		}
};
	
}
