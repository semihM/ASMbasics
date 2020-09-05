#pragma once

namespace CLRWinForms {
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
		Bitmap^ temp;
		unsigned char* bmpOriginal;
		unsigned char* hBlur;
		unsigned char* keptIMG;
		unsigned char* HSL;
		unsigned char* temprgb;
		unsigned char* temphsl;
		unsigned char* HSL_RANGES;
		unsigned char* HSL_RANGES_temp;
		unsigned char* HSL_COLORS;
		unsigned char* temp_nrgb;
		unsigned int* x_y;
		unsigned char* rgb_pickertool;

		unsigned char* scan0;
		static unsigned int change_index;
		static int widthinbytes;
		static int imgSizeInBytes = -1;
		static Rectangle imgRect;
		BitmapData^ bmpData;

		static int saveCounter = 1;
		static String ^fileName;
		static String^ lastAction = "";

		static Color sColor, nColor, pickedPx;
		static Pen^ drawPen;

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
		static double Move_cppTotal = 0.0;
		static double Move_cppCount = 0.0;
		static double Move_asmCount = 0.0;
		static double Move_asmTotal = 0.0;

		static int boxW, boxH, dispW, dispH, realW, realH, whiteSpaceW, whiteSpaceH, X, Y;
		static double hRatio, wRatio, minRatio;
		static int onImage = 1;
		static int ratioIndex;

		static bool usingPickerPointer = false;
		static int pickerIndex = -1;

		static String^ pickedColorName;
		static int ticks = 0;
		static long startMove, finishMove;

		static System::Windows::Forms::MouseButtons leftbutton = System::Windows::Forms::MouseButtons::Left;

		void CPPCoordinates(unsigned int* xyo, int rIndex, int whiteW, int whiteH, int dW, int dH, int bW, int bH)
		{
			
			if (rIndex == 0)
			{
				if (xyo[0] > dW + whiteW || xyo[0] <= whiteW || xyo[1] <= 0 || xyo[1] > bH)
				{
					xyo[0] = 0;
					xyo[1] = 0;
					xyo[2] = 0;
				}
				else
				{
					xyo[0] -= whiteW;
					xyo[2] = 1;
				}
			}

			else if (rIndex)
			{
				if (xyo[1] > dH + whiteH || xyo[1] <= whiteH || xyo[0] <= 0 || xyo[0] > bW)
				{
					xyo[0] = 0;
					xyo[1] = 0;
					xyo[2] = 0;
				}
				else
				{
					xyo[1] -= whiteH;
					xyo[2] = 1;
				}
			}
		}

		void ResetIMG(unsigned char* bmp, unsigned char* original, long imgSize)
		{
			for (int i = 0; i < imgSize; i++)
			{
				bmp[i] = original[i];
			}
		}

		void resetAll()
		{
			averageTimeLabel->Text = "Average (ms) :-";

			brightnessTrackbar->Value = 0;
			brightnessLabel->Text = "Brightness";

			blurTrackbar->Value = 0;
			blurLabel->Text = "Blur Width";

			rangeAboveScroll->Value = 0;
			rangeAboveScroll->Maximum = 240;
			rangeBelowScroll->Value = 0;
			rangeBelowScroll->Minimum = -240;
			belowLabel->Text = "-0";
			aboveLabel->Text = "+0";
			belowLabel->BackColor = Color::CadetBlue;
			belowLabel->ForeColor = Color::White;
			aboveLabel->BackColor = Color::CadetBlue;
			aboveLabel->ForeColor = Color::White;

			rangeDropdown->ResetText();

			searchColorText->ResetText();
			searchColorText->BackColor = Color::White;
			searchColorText->ForeColor = Color::Black;

			newColorText->ResetText();
			newColorText->BackColor = Color::White;
			newColorText->ForeColor = Color::Black;

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
			Move_cppCount = 0.0;
			Move_cppTotal = 0.0;
			Move_asmCount = 0.0;
			Move_asmTotal = 0.0;

			lastAction = "";

			keptIMG = new unsigned char[imgSizeInBytes];

			rangeMid->ResetBackColor();
			rangeMid->ResetText();
			rangeMid->ResetForeColor();
			HSL = new unsigned char[3]{ 0,0,0 };
			HSL_RANGES = new unsigned char[6]{ 0,0, 0,0, 0,0 };
			HSL_COLORS = new unsigned char[18]{ 0,0,0, 0,0,0,
												0,0,0, 0,0,0,
												0,0,0, 0,0,0 };

			colorPickerToolHexText->ResetText();
			colorPickerToolHexText->BackColor = Color::White;
			colorPickerToolHexText->ForeColor = Color::Black;

			pickerIndex = -1;
			usingPickerPointer = false;

			colorPickToolCheckbox->Checked = false;
			searchPickToolCheckbox->Checked = false;
			newPickToolCheckbox->Checked = false;
			drawPenCheckbox->Enabled = false;
			drawPenCheckbox->Checked = false;

			onImage = 1;
		}

		void AdjustBrightness(unsigned char* bmp,unsigned char* org, short amount)
		{
			for (int i = 0; i < imgSizeInBytes; i++)
			{
				if ((short)org[i] + amount < 0) bmp[i] = 0;
				else if ((short)org[i] + amount > 255) bmp[i] = 255;
				else bmp[i] = org[i] + amount;
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

		void CPPColorChangeColorRange(unsigned char* bmp,
									  Color newColor, 
									  unsigned char* ranges,
									  int imgsize)
		{	

			// Destination color
			unsigned char Db = newColor.B;
			unsigned char Dg = newColor.G;
			unsigned char Dr = newColor.R;

			temphsl = new unsigned char[3];
			unsigned char temphue,tempsat,templum;
			 
			unsigned char hueL,hueH,satL,satH,lumL,lumH;
			hueL = ranges[0];
			hueH = ranges[1];
			satL = ranges[2];
			satH = ranges[3];
			lumL = ranges[4];
			lumH = ranges[5];

			for (int p = 0; p < imgsize; p += 3)
			{
				convert2HSL((float)bmp[p+2], (float)bmp[p + 1], (float)bmp[p], temphsl);
				temphue = temphsl[0];
				tempsat = temphsl[1];
				templum = temphsl[2];
				if ((tempsat <= satH && tempsat >= satL) &&
					(temphue <= hueH && temphue >= hueL) &&
					(templum <= lumH && templum >= lumL))
				{
					bmp[p] = Db;
					bmp[p + 1] = Dg;
					bmp[p + 2] = Dr;
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
		
		void convert2RGB(float h, float s, float l,unsigned char* rgb)
		{
			float r, g, b, q, p;
			
			h /= 240.0;
			s /= 240.0;
			l /= 240.0;

			if (s == 0.0) 
			{
				r = g = b = l;
			}
			else 
			{
				q = l < 0.5 ? l * (1 + s) : l + s - l * s;
				p = 2 * l - q;
				r = torgb(p, q, h + 0.3333333333);
				g = torgb(p, q, h);
				b = torgb(p, q, h - 0.3333333333);
			}
			rgb[0] = (unsigned char)Math::Round(r * 255);
			rgb[1] = (unsigned char)Math::Round(g * 255);
			rgb[2] = (unsigned char)Math::Round(b * 255);
		}

		float torgb(float d, float e, float c)
		{
			if (c < 0.0)
			{
				c += 1.0;
			}
			else if (c > 1.0)
			{
				c -= 1.0;
			}
			if (c < 0.166666666)
			{
				return d + (e - d) * 6.0 * c;
			}
			if (c < 0.5)
			{
				return e;
			}
			if (c < 0.6666666666)
			{
				return d + (e - d) * (0.6666666666 - c) * 6.0;
			}
			return d;
		}

		void convert2HSL(float r, float g, float b, unsigned char * hsl)
		{	
			double rN, gN, bN, max, min, delta, h, s ,l;
			
			rN = r / 255; gN = g / 255; bN = b / 255;
			
			max = rN + (gN - rN) * (gN > rN); max = max + (bN - max) * (bN > max);
			min = rN + (gN - rN) * (gN < rN); min = min + (bN - min) * (bN < min);

			l = (max + min) * 0.5;

			if(max == min)
			{
				hsl[0] = 0;
				hsl[1] = 0;
				hsl[2] = (unsigned char)(l * 240.0);
			}
			else 
			{
				delta = max - min;
				s = l > 0.5 ? delta / (2.0 - max - min) : delta / (max + min);
				
				if (max == rN)
					h = (gN - bN) / delta + (gN < bN ? 6.0 : 0);
				else if (max == gN)
					h = (bN - rN) / delta + 2.0;
				else if (max == bN)
					h = (rN - gN) / delta + 4.0;
				else
					h = 0;
			
				h /= 6.0;
				hsl[0] = (unsigned char)(h*240.0);
				hsl[1] = (unsigned char)(s*240.0);
				hsl[2] = (unsigned char)(l*240.0);

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
	private: System::Windows::Forms::MenuStrip^ mainBox;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;

	private: System::Windows::Forms::PictureBox^ pictureBoxImg;
	private: System::Windows::Forms::Label^ averageTimeLabel;

	private: System::Windows::Forms::OpenFileDialog^ dlgOpen;

	private: System::Windows::Forms::TrackBar^ brightnessTrackbar;
	private: System::Windows::Forms::Label^ brightnessLabel;

	private: System::Windows::Forms::Label^ effectsLabel;


	private: System::Windows::Forms::TrackBar^ blurTrackbar;
	private: System::Windows::Forms::Label^ blurLabel;

	private: System::Windows::Forms::CheckBox^ ASMCheckBox;


	private: System::Windows::Forms::Label^ searchColorLabel;
	private: System::Windows::Forms::Label^ newColorLabel;
	private: System::Windows::Forms::ColorDialog^ SearchColorDiag;
	private: System::Windows::Forms::ColorDialog^ newColorDiag;
	private: System::Windows::Forms::TextBox^ searchColorText;
	private: System::Windows::Forms::TextBox^ newColorText;

	private: System::Windows::Forms::Button^ searchColorButton;
	private: System::Windows::Forms::Button^ newColorButton;
	private: System::Windows::Forms::ColorDialog^ searchColorChangeDialog;
	private: System::Windows::Forms::ColorDialog^ newColorChangeDialog;
	private: System::Windows::Forms::ColorDialog^ toolColorChangeDialog;



	private: System::Windows::Forms::Button^ colorApplyButton;
	private: System::Windows::Forms::Label^ rangeBigLabel;

	private: System::Windows::Forms::TextBox^ rangeMid;
	private: System::Windows::Forms::ComboBox^ rangeDropdown;
	private: System::Windows::Forms::Label^ rangeTypeLabel;
	private: System::Windows::Forms::TrackBar^ rangeBelowScroll;
	private: System::Windows::Forms::TrackBar^ rangeAboveScroll;
	private: System::Windows::Forms::Label^ belowLabel;
	private: System::Windows::Forms::Label^ aboveLabel;

	private: System::Windows::Forms::Button^ greyscaleButton;
	private: System::Windows::Forms::Button^ negativeButton;
	private: System::Windows::Forms::Button^ resetButton;
	private: System::Windows::Forms::CheckBox^ keepIMGCheckBox;

	private: System::Windows::Forms::ToolStripMenuItem^ saveMenuItem;
	private: System::Windows::Forms::SaveFileDialog^ saveFileDialog;

	private: System::Windows::Forms::TextBox^ pointerInfoText;

	private: System::Windows::Forms::Label^ toolsLabel;

	private: System::Windows::Forms::CheckBox^ colorPickToolCheckbox;
	private: System::Windows::Forms::CheckBox^ searchPickToolCheckbox;
	private: System::Windows::Forms::CheckBox^ newPickToolCheckbox;

	private: System::Windows::Forms::TextBox^ colorPickerToolHexText;

	private: System::Windows::Forms::CheckBox^ drawPenCheckbox;
	private: System::Windows::Forms::Button^ toolPickdiagButton;
	private: System::Windows::Forms::Label^ toolColorLabel;	
	private: System::Windows::Forms::Label^ toolColorUnderlineLabel;


	private: System::ComponentModel::Container^ components;
	

	#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Erforderliche Methode für die Designerunterstützung.
		   /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			   this->mainBox = (gcnew System::Windows::Forms::MenuStrip());
			   this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->saveMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			   this->pictureBoxImg = (gcnew System::Windows::Forms::PictureBox());
			   this->averageTimeLabel = (gcnew System::Windows::Forms::Label());
			   this->dlgOpen = (gcnew System::Windows::Forms::OpenFileDialog());
			   this->brightnessTrackbar = (gcnew System::Windows::Forms::TrackBar());
			   this->brightnessLabel = (gcnew System::Windows::Forms::Label());
			   this->effectsLabel = (gcnew System::Windows::Forms::Label());
			   this->blurTrackbar = (gcnew System::Windows::Forms::TrackBar());
			   this->blurLabel = (gcnew System::Windows::Forms::Label());
			   this->ASMCheckBox = (gcnew System::Windows::Forms::CheckBox());
			   this->searchColorLabel = (gcnew System::Windows::Forms::Label());
			   this->newColorLabel = (gcnew System::Windows::Forms::Label());
			   this->searchColorText = (gcnew System::Windows::Forms::TextBox());
			   this->newColorText = (gcnew System::Windows::Forms::TextBox());
			   this->searchColorButton = (gcnew System::Windows::Forms::Button());
			   this->newColorButton = (gcnew System::Windows::Forms::Button());
			   this->searchColorChangeDialog = (gcnew System::Windows::Forms::ColorDialog());
			   this->colorApplyButton = (gcnew System::Windows::Forms::Button());
			   this->resetButton = (gcnew System::Windows::Forms::Button());
			   this->keepIMGCheckBox = (gcnew System::Windows::Forms::CheckBox());
			   this->saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			   this->greyscaleButton = (gcnew System::Windows::Forms::Button());
			   this->negativeButton = (gcnew System::Windows::Forms::Button());
			   this->rangeBigLabel = (gcnew System::Windows::Forms::Label());
			   this->rangeMid = (gcnew System::Windows::Forms::TextBox());
			   this->rangeDropdown = (gcnew System::Windows::Forms::ComboBox());
			   this->rangeTypeLabel = (gcnew System::Windows::Forms::Label());
			   this->rangeBelowScroll = (gcnew System::Windows::Forms::TrackBar());
			   this->rangeAboveScroll = (gcnew System::Windows::Forms::TrackBar());
			   this->belowLabel = (gcnew System::Windows::Forms::Label());
			   this->aboveLabel = (gcnew System::Windows::Forms::Label());
			   this->pointerInfoText = (gcnew System::Windows::Forms::TextBox());
			   this->toolsLabel = (gcnew System::Windows::Forms::Label());
			   this->colorPickerToolHexText = (gcnew System::Windows::Forms::TextBox());
			   this->colorPickToolCheckbox = (gcnew System::Windows::Forms::CheckBox());
			   this->searchPickToolCheckbox = (gcnew System::Windows::Forms::CheckBox());
			   this->newPickToolCheckbox = (gcnew System::Windows::Forms::CheckBox());
			   this->drawPenCheckbox = (gcnew System::Windows::Forms::CheckBox());
			   this->toolPickdiagButton = (gcnew System::Windows::Forms::Button());
			   this->toolColorLabel = (gcnew System::Windows::Forms::Label());
			   this->toolColorUnderlineLabel = (gcnew System::Windows::Forms::Label());
			   this->newColorChangeDialog = (gcnew System::Windows::Forms::ColorDialog());
			   this->toolColorChangeDialog = (gcnew System::Windows::Forms::ColorDialog());
			   this->mainBox->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->brightnessTrackbar))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurTrackbar))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->rangeBelowScroll))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->rangeAboveScroll))->BeginInit();
			   this->SuspendLayout();
			   this->mainBox->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			   this->mainBox->Location = System::Drawing::Point(0, 0);
			   this->mainBox->Name = L"mainBox";
			   this->mainBox->Size = System::Drawing::Size(711, 24);
			   this->mainBox->TabIndex = 0;
			   this->mainBox->Text = L"mainBox";
			   this->mainBox->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &Form1::mainBox_ItemClicked);
			   this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				   this->openToolStripMenuItem,
					   this->saveMenuItem, this->exitToolStripMenuItem
			   });
			   this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			   this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			   this->fileToolStripMenuItem->Text = L"File";
			   this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			   this->openToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			   this->openToolStripMenuItem->Text = L"&Open";
			   this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			   this->saveMenuItem->Enabled = false;
			   this->saveMenuItem->Name = L"saveMenuItem";
			   this->saveMenuItem->Size = System::Drawing::Size(180, 22);
			   this->saveMenuItem->Text = L"&Save";
			   this->saveMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveMenuItem_Click);
			   this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			   this->exitToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			   this->exitToolStripMenuItem->Text = L"&Exit";
			   this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			   this->pictureBoxImg->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				   | System::Windows::Forms::AnchorStyles::Left)
				   | System::Windows::Forms::AnchorStyles::Right));
			   this->pictureBoxImg->BackColor = System::Drawing::SystemColors::Control;
			   this->pictureBoxImg->Cursor = System::Windows::Forms::Cursors::Cross;
			   this->pictureBoxImg->Location = System::Drawing::Point(0, 27);
			   this->pictureBoxImg->Name = L"pictureBoxImg";
			   this->pictureBoxImg->Size = System::Drawing::Size(711, 413);
			   this->pictureBoxImg->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBoxImg->TabIndex = 1;
			   this->pictureBoxImg->TabStop = false;
			   this->pictureBoxImg->Click += gcnew System::EventHandler(this, &Form1::pictureBoxImg_Click);
			   this->pictureBoxImg->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBoxImg_MouseMove);
			   this->averageTimeLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->averageTimeLabel->AutoSize = true;
			   this->averageTimeLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->averageTimeLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->averageTimeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->averageTimeLabel->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->averageTimeLabel->Location = System::Drawing::Point(13, 665);
			   this->averageTimeLabel->Margin = System::Windows::Forms::Padding(2);
			   this->averageTimeLabel->Name = L"averageTimeLabel";
			   this->averageTimeLabel->Padding = System::Windows::Forms::Padding(1, 1, 1, 3);
			   this->averageTimeLabel->Size = System::Drawing::Size(133, 24);
			   this->averageTimeLabel->TabIndex = 3;
			   this->averageTimeLabel->Text = L"Average (ms) : -";
			   this->averageTimeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			   this->averageTimeLabel->Click += gcnew System::EventHandler(this, &Form1::toolColorLabel_Click);
			   this->dlgOpen->FileName = L"openFileDialog1";
			   this->dlgOpen->Filter = L"JPEG|*.jpg|PNG|*.png|Bitmap|*.bmp|All Files|*.*";
			   this->brightnessTrackbar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->brightnessTrackbar->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->brightnessTrackbar->Enabled = false;
			   this->brightnessTrackbar->Location = System::Drawing::Point(12, 467);
			   this->brightnessTrackbar->Maximum = 255;
			   this->brightnessTrackbar->Minimum = -255;
			   this->brightnessTrackbar->Name = L"brightnessTrackbar";
			   this->brightnessTrackbar->Size = System::Drawing::Size(206, 45);
			   this->brightnessTrackbar->TabIndex = 4;
			   this->brightnessTrackbar->TickFrequency = 16;
			   this->brightnessTrackbar->Scroll += gcnew System::EventHandler(this, &Form1::brightnessTrackbar_Scroll);
			   this->brightnessLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->brightnessLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->brightnessLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->brightnessLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->brightnessLabel->ForeColor = System::Drawing::Color::White;
			   this->brightnessLabel->Location = System::Drawing::Point(12, 441);
			   this->brightnessLabel->Name = L"brightnessLabel";
			   this->brightnessLabel->Size = System::Drawing::Size(206, 26);
			   this->brightnessLabel->TabIndex = 10;
			   this->brightnessLabel->Text = L"Brightness";
			   this->brightnessLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->effectsLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->effectsLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->effectsLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->effectsLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->effectsLabel->ForeColor = System::Drawing::Color::White;
			   this->effectsLabel->Location = System::Drawing::Point(236, 441);
			   this->effectsLabel->Name = L"effectsLabel";
			   this->effectsLabel->Size = System::Drawing::Size(78, 26);
			   this->effectsLabel->TabIndex = 11;
			   this->effectsLabel->Text = L"Effects";
			   this->effectsLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->blurTrackbar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->blurTrackbar->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->blurTrackbar->Enabled = false;
			   this->blurTrackbar->LargeChange = 10;
			   this->blurTrackbar->Location = System::Drawing::Point(12, 541);
			   this->blurTrackbar->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
			   this->blurTrackbar->Maximum = 100;
			   this->blurTrackbar->Name = L"blurTrackbar";
			   this->blurTrackbar->Size = System::Drawing::Size(206, 45);
			   this->blurTrackbar->TabIndex = 1;
			   this->blurTrackbar->TickFrequency = 10;
			   this->blurTrackbar->Scroll += gcnew System::EventHandler(this, &Form1::blurTrackbar_Scroll);
			   this->blurLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->blurLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->blurLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->blurLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->blurLabel->ForeColor = System::Drawing::Color::White;
			   this->blurLabel->Location = System::Drawing::Point(12, 515);
			   this->blurLabel->Name = L"blurLabel";
			   this->blurLabel->Size = System::Drawing::Size(206, 26);
			   this->blurLabel->TabIndex = 14;
			   this->blurLabel->Text = L"Blur Width";
			   this->blurLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->ASMCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->ASMCheckBox->AutoSize = true;
			   this->ASMCheckBox->BackColor = System::Drawing::Color::DarkSlateGray;
			   this->ASMCheckBox->Enabled = false;
			   this->ASMCheckBox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->ASMCheckBox->FlatAppearance->CheckedBackColor = System::Drawing::Color::Lime;
			   this->ASMCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->ASMCheckBox->ForeColor = System::Drawing::Color::White;
			   this->ASMCheckBox->Location = System::Drawing::Point(12, 617);
			   this->ASMCheckBox->Name = L"ASMCheckBox";
			   this->ASMCheckBox->Size = System::Drawing::Size(78, 17);
			   this->ASMCheckBox->TabIndex = 15;
			   this->ASMCheckBox->Text = L"Use ASM";
			   this->ASMCheckBox->UseVisualStyleBackColor = false;
			   this->searchColorLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->searchColorLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->searchColorLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->searchColorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->searchColorLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->searchColorLabel->Location = System::Drawing::Point(514, 441);
			   this->searchColorLabel->Name = L"searchColorLabel";
			   this->searchColorLabel->Size = System::Drawing::Size(92, 18);
			   this->searchColorLabel->TabIndex = 19;
			   this->searchColorLabel->Text = L"Search Color";
			   this->searchColorLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->newColorLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->newColorLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->newColorLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->newColorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->newColorLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->newColorLabel->Location = System::Drawing::Point(612, 441);
			   this->newColorLabel->Name = L"newColorLabel";
			   this->newColorLabel->Size = System::Drawing::Size(89, 18);
			   this->newColorLabel->TabIndex = 20;
			   this->newColorLabel->Text = L"New Color";
			   this->newColorLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->searchColorText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->searchColorText->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->searchColorText->Location = System::Drawing::Point(514, 462);
			   this->searchColorText->Name = L"searchColorText";
			   this->searchColorText->ReadOnly = true;
			   this->searchColorText->Size = System::Drawing::Size(92, 20);
			   this->searchColorText->TabIndex = 21;
			   this->searchColorText->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->newColorText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->newColorText->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->newColorText->Location = System::Drawing::Point(612, 462);
			   this->newColorText->Name = L"newColorText";
			   this->newColorText->ReadOnly = true;
			   this->newColorText->Size = System::Drawing::Size(89, 20);
			   this->newColorText->TabIndex = 22;
			   this->newColorText->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->searchColorButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->searchColorButton->CausesValidation = false;
			   this->searchColorButton->Enabled = false;
			   this->searchColorButton->Location = System::Drawing::Point(514, 488);
			   this->searchColorButton->Name = L"searchColorButton";
			   this->searchColorButton->Size = System::Drawing::Size(63, 24);
			   this->searchColorButton->TabIndex = 23;
			   this->searchColorButton->Text = L"Pick";
			   this->searchColorButton->UseVisualStyleBackColor = true;
			   this->searchColorButton->Click += gcnew System::EventHandler(this, &Form1::searchColorButton_Click);
			   this->newColorButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->newColorButton->Enabled = false;
			   this->newColorButton->Location = System::Drawing::Point(612, 488);
			   this->newColorButton->Name = L"newColorButton";
			   this->newColorButton->Size = System::Drawing::Size(61, 24);
			   this->newColorButton->TabIndex = 24;
			   this->newColorButton->Text = L"Pick";
			   this->newColorButton->UseVisualStyleBackColor = true;
			   this->newColorButton->Click += gcnew System::EventHandler(this, &Form1::newColorButton_Click);
			   this->searchColorChangeDialog->AnyColor = true;
			   this->searchColorChangeDialog->FullOpen = true;
			   this->searchColorChangeDialog->SolidColorOnly = true;
			   this->colorApplyButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->colorApplyButton->CausesValidation = false;
			   this->colorApplyButton->Enabled = false;
			   this->colorApplyButton->Location = System::Drawing::Point(514, 636);
			   this->colorApplyButton->Name = L"colorApplyButton";
			   this->colorApplyButton->Size = System::Drawing::Size(187, 24);
			   this->colorApplyButton->TabIndex = 25;
			   this->colorApplyButton->Text = L"Change Colors";
			   this->colorApplyButton->UseVisualStyleBackColor = true;
			   this->colorApplyButton->Click += gcnew System::EventHandler(this, &Form1::colorApplyButton_Click);
			   this->resetButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->resetButton->BackColor = System::Drawing::Color::Turquoise;
			   this->resetButton->Enabled = false;
			   this->resetButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->resetButton->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->resetButton->Location = System::Drawing::Point(12, 640);
			   this->resetButton->Name = L"resetButton";
			   this->resetButton->Size = System::Drawing::Size(78, 21);
			   this->resetButton->TabIndex = 26;
			   this->resetButton->Text = L"RESET";
			   this->resetButton->UseVisualStyleBackColor = false;
			   this->resetButton->Click += gcnew System::EventHandler(this, &Form1::resetButton_Click);
			   this->keepIMGCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->keepIMGCheckBox->AutoSize = true;
			   this->keepIMGCheckBox->BackColor = System::Drawing::Color::DarkSlateGray;
			   this->keepIMGCheckBox->Enabled = false;
			   this->keepIMGCheckBox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->keepIMGCheckBox->FlatAppearance->CheckedBackColor = System::Drawing::Color::Lime;
			   this->keepIMGCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->keepIMGCheckBox->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			   this->keepIMGCheckBox->Location = System::Drawing::Point(12, 592);
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
			   this->greyscaleButton->Location = System::Drawing::Point(236, 500);
			   this->greyscaleButton->Name = L"greyscaleButton";
			   this->greyscaleButton->Size = System::Drawing::Size(78, 24);
			   this->greyscaleButton->TabIndex = 28;
			   this->greyscaleButton->Text = L"Greyscale";
			   this->greyscaleButton->UseVisualStyleBackColor = true;
			   this->greyscaleButton->Click += gcnew System::EventHandler(this, &Form1::greyscaleButton_Click);
			   this->negativeButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			   this->negativeButton->CausesValidation = false;
			   this->negativeButton->Enabled = false;
			   this->negativeButton->Location = System::Drawing::Point(236, 470);
			   this->negativeButton->Name = L"negativeButton";
			   this->negativeButton->Size = System::Drawing::Size(78, 24);
			   this->negativeButton->TabIndex = 29;
			   this->negativeButton->Text = L"Negative";
			   this->negativeButton->UseVisualStyleBackColor = true;
			   this->negativeButton->Click += gcnew System::EventHandler(this, &Form1::negativeButton_Click);
			   this->rangeBigLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeBigLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->rangeBigLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->rangeBigLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->rangeBigLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->rangeBigLabel->Location = System::Drawing::Point(514, 515);
			   this->rangeBigLabel->Name = L"rangeBigLabel";
			   this->rangeBigLabel->Size = System::Drawing::Size(187, 18);
			   this->rangeBigLabel->TabIndex = 31;
			   this->rangeBigLabel->Text = L"HSL Ranges";
			   this->rangeBigLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->rangeMid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeMid->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->rangeMid->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->rangeMid->Location = System::Drawing::Point(578, 564);
			   this->rangeMid->Name = L"rangeMid";
			   this->rangeMid->ReadOnly = true;
			   this->rangeMid->Size = System::Drawing::Size(62, 20);
			   this->rangeMid->TabIndex = 33;
			   this->rangeMid->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->rangeDropdown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeDropdown->Enabled = false;
			   this->rangeDropdown->FormattingEnabled = true;
			   this->rangeDropdown->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Hue", L"Sat", L"Lum" });
			   this->rangeDropdown->Location = System::Drawing::Point(621, 537);
			   this->rangeDropdown->Name = L"rangeDropdown";
			   this->rangeDropdown->Size = System::Drawing::Size(80, 21);
			   this->rangeDropdown->TabIndex = 34;
			   this->rangeDropdown->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::rangeDropdown_SelectedIndexChanged);
			   this->rangeTypeLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeTypeLabel->BackColor = System::Drawing::Color::CadetBlue;
			   this->rangeTypeLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->rangeTypeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->rangeTypeLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->rangeTypeLabel->Location = System::Drawing::Point(514, 537);
			   this->rangeTypeLabel->Name = L"rangeTypeLabel";
			   this->rangeTypeLabel->Size = System::Drawing::Size(104, 21);
			   this->rangeTypeLabel->TabIndex = 35;
			   this->rangeTypeLabel->Text = L"Type";
			   this->rangeTypeLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->rangeBelowScroll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeBelowScroll->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->rangeBelowScroll->Enabled = false;
			   this->rangeBelowScroll->Location = System::Drawing::Point(514, 585);
			   this->rangeBelowScroll->Maximum = 0;
			   this->rangeBelowScroll->Minimum = -240;
			   this->rangeBelowScroll->Name = L"rangeBelowScroll";
			   this->rangeBelowScroll->Size = System::Drawing::Size(63, 45);
			   this->rangeBelowScroll->TabIndex = 36;
			   this->rangeBelowScroll->TickFrequency = 16;
			   this->rangeBelowScroll->Scroll += gcnew System::EventHandler(this, &Form1::rangeBelowScroll_Scroll);
			   this->rangeAboveScroll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->rangeAboveScroll->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			   this->rangeAboveScroll->Enabled = false;
			   this->rangeAboveScroll->Location = System::Drawing::Point(641, 585);
			   this->rangeAboveScroll->Maximum = 240;
			   this->rangeAboveScroll->Name = L"rangeAboveScroll";
			   this->rangeAboveScroll->Size = System::Drawing::Size(60, 45);
			   this->rangeAboveScroll->TabIndex = 37;
			   this->rangeAboveScroll->TickFrequency = 16;
			   this->rangeAboveScroll->Scroll += gcnew System::EventHandler(this, &Form1::rangeAboveScroll_Scroll);
			   this->belowLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->belowLabel->BackColor = System::Drawing::Color::CadetBlue;
			   this->belowLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->belowLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->belowLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->belowLabel->Location = System::Drawing::Point(514, 564);
			   this->belowLabel->Name = L"belowLabel";
			   this->belowLabel->Size = System::Drawing::Size(63, 22);
			   this->belowLabel->TabIndex = 38;
			   this->belowLabel->Text = L"-0";
			   this->belowLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->aboveLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->aboveLabel->BackColor = System::Drawing::Color::CadetBlue;
			   this->aboveLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->aboveLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->aboveLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->aboveLabel->Location = System::Drawing::Point(641, 564);
			   this->aboveLabel->Name = L"aboveLabel";
			   this->aboveLabel->Size = System::Drawing::Size(60, 22);
			   this->aboveLabel->TabIndex = 39;
			   this->aboveLabel->Text = L"+0";
			   this->aboveLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->pointerInfoText->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			   this->pointerInfoText->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(246)), static_cast<System::Int32>(static_cast<System::Byte>(246)),
				   static_cast<System::Int32>(static_cast<System::Byte>(246)));
			   this->pointerInfoText->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->pointerInfoText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->pointerInfoText->Location = System::Drawing::Point(457, 3);
			   this->pointerInfoText->Multiline = true;
			   this->pointerInfoText->Name = L"pointerInfoText";
			   this->pointerInfoText->ReadOnly = true;
			   this->pointerInfoText->Size = System::Drawing::Size(244, 21);
			   this->pointerInfoText->TabIndex = 40;
			   this->pointerInfoText->Text = L"Pos:(0,0) - Real:(0,0)";
			   this->pointerInfoText->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			   this->toolsLabel->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->toolsLabel->BackColor = System::Drawing::Color::DarkCyan;
			   this->toolsLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->toolsLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->toolsLabel->ForeColor = System::Drawing::Color::White;
			   this->toolsLabel->Location = System::Drawing::Point(375, 441);
			   this->toolsLabel->Name = L"toolsLabel";
			   this->toolsLabel->Size = System::Drawing::Size(72, 26);
			   this->toolsLabel->TabIndex = 41;
			   this->toolsLabel->Text = L"Tools";
			   this->toolsLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->colorPickerToolHexText->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->colorPickerToolHexText->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->colorPickerToolHexText->Location = System::Drawing::Point(375, 492);
			   this->colorPickerToolHexText->Name = L"colorPickerToolHexText";
			   this->colorPickerToolHexText->ReadOnly = true;
			   this->colorPickerToolHexText->Size = System::Drawing::Size(72, 20);
			   this->colorPickerToolHexText->TabIndex = 45;
			   this->colorPickerToolHexText->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			   this->colorPickToolCheckbox->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->colorPickToolCheckbox->Appearance = System::Windows::Forms::Appearance::Button;
			   this->colorPickToolCheckbox->BackColor = System::Drawing::Color::Red;
			   this->colorPickToolCheckbox->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"colorPickToolCheckbox.BackgroundImage")));
			   this->colorPickToolCheckbox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			   this->colorPickToolCheckbox->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->colorPickToolCheckbox->Enabled = false;
			   this->colorPickToolCheckbox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->colorPickToolCheckbox->FlatAppearance->BorderSize = 3;
			   this->colorPickToolCheckbox->FlatAppearance->CheckedBackColor = System::Drawing::Color::GreenYellow;
			   this->colorPickToolCheckbox->Location = System::Drawing::Point(421, 518);
			   this->colorPickToolCheckbox->Name = L"colorPickToolCheckbox";
			   this->colorPickToolCheckbox->Size = System::Drawing::Size(26, 24);
			   this->colorPickToolCheckbox->TabIndex = 46;
			   this->colorPickToolCheckbox->UseVisualStyleBackColor = false;
			   this->colorPickToolCheckbox->CheckedChanged += gcnew System::EventHandler(this, &Form1::colorPickToolCheckbox_CheckedChanged);
			   this->searchPickToolCheckbox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->searchPickToolCheckbox->Appearance = System::Windows::Forms::Appearance::Button;
			   this->searchPickToolCheckbox->BackColor = System::Drawing::Color::Red;
			   this->searchPickToolCheckbox->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"searchPickToolCheckbox.BackgroundImage")));
			   this->searchPickToolCheckbox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			   this->searchPickToolCheckbox->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->searchPickToolCheckbox->Enabled = false;
			   this->searchPickToolCheckbox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->searchPickToolCheckbox->FlatAppearance->BorderSize = 3;
			   this->searchPickToolCheckbox->FlatAppearance->CheckedBackColor = System::Drawing::Color::GreenYellow;
			   this->searchPickToolCheckbox->Location = System::Drawing::Point(580, 488);
			   this->searchPickToolCheckbox->Name = L"searchPickToolCheckbox";
			   this->searchPickToolCheckbox->Size = System::Drawing::Size(26, 24);
			   this->searchPickToolCheckbox->TabIndex = 47;
			   this->searchPickToolCheckbox->UseVisualStyleBackColor = false;
			   this->searchPickToolCheckbox->CheckedChanged += gcnew System::EventHandler(this, &Form1::searchPickToolCheckbox_CheckedChanged);
			   this->newPickToolCheckbox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			   this->newPickToolCheckbox->Appearance = System::Windows::Forms::Appearance::Button;
			   this->newPickToolCheckbox->BackColor = System::Drawing::Color::Red;
			   this->newPickToolCheckbox->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"newPickToolCheckbox.BackgroundImage")));
			   this->newPickToolCheckbox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			   this->newPickToolCheckbox->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->newPickToolCheckbox->Enabled = false;
			   this->newPickToolCheckbox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->newPickToolCheckbox->FlatAppearance->BorderSize = 3;
			   this->newPickToolCheckbox->FlatAppearance->CheckedBackColor = System::Drawing::Color::GreenYellow;
			   this->newPickToolCheckbox->Location = System::Drawing::Point(675, 488);
			   this->newPickToolCheckbox->Name = L"newPickToolCheckbox";
			   this->newPickToolCheckbox->Size = System::Drawing::Size(26, 24);
			   this->newPickToolCheckbox->TabIndex = 48;
			   this->newPickToolCheckbox->UseVisualStyleBackColor = false;
			   this->newPickToolCheckbox->CheckedChanged += gcnew System::EventHandler(this, &Form1::newPickToolCheckbox_CheckedChanged);
			   this->drawPenCheckbox->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->drawPenCheckbox->Appearance = System::Windows::Forms::Appearance::Button;
			   this->drawPenCheckbox->BackColor = System::Drawing::Color::Red;
			   this->drawPenCheckbox->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"drawPenCheckbox.BackgroundImage")));
			   this->drawPenCheckbox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			   this->drawPenCheckbox->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->drawPenCheckbox->Enabled = false;
			   this->drawPenCheckbox->FlatAppearance->BorderColor = System::Drawing::Color::White;
			   this->drawPenCheckbox->FlatAppearance->BorderSize = 3;
			   this->drawPenCheckbox->FlatAppearance->CheckedBackColor = System::Drawing::Color::GreenYellow;
			   this->drawPenCheckbox->Location = System::Drawing::Point(375, 558);
			   this->drawPenCheckbox->Name = L"drawPenCheckbox";
			   this->drawPenCheckbox->Size = System::Drawing::Size(26, 24);
			   this->drawPenCheckbox->TabIndex = 49;
			   this->drawPenCheckbox->UseVisualStyleBackColor = false;
			   this->drawPenCheckbox->CheckedChanged += gcnew System::EventHandler(this, &Form1::drawPenCheckbox_CheckedChanged);
			   this->toolPickdiagButton->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->toolPickdiagButton->CausesValidation = false;
			   this->toolPickdiagButton->Enabled = false;
			   this->toolPickdiagButton->Location = System::Drawing::Point(375, 517);
			   this->toolPickdiagButton->Name = L"toolPickdiagButton";
			   this->toolPickdiagButton->Size = System::Drawing::Size(40, 25);
			   this->toolPickdiagButton->TabIndex = 50;
			   this->toolPickdiagButton->Text = L"Pick";
			   this->toolPickdiagButton->UseVisualStyleBackColor = true;
			   this->toolPickdiagButton->Click += gcnew System::EventHandler(this, &Form1::toolPickdiagButton_Click);
			   this->toolColorLabel->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->toolColorLabel->BackColor = System::Drawing::Color::CadetBlue;
			   this->toolColorLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->toolColorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->toolColorLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->toolColorLabel->Location = System::Drawing::Point(375, 470);
			   this->toolColorLabel->Name = L"toolColorLabel";
			   this->toolColorLabel->Size = System::Drawing::Size(72, 19);
			   this->toolColorLabel->TabIndex = 51;
			   this->toolColorLabel->Text = L"Color";
			   this->toolColorLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->toolColorUnderlineLabel->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			   this->toolColorUnderlineLabel->BackColor = System::Drawing::Color::DarkSlateGray;
			   this->toolColorUnderlineLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->toolColorUnderlineLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular,
				   System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			   this->toolColorUnderlineLabel->ForeColor = System::Drawing::SystemColors::ButtonFace;
			   this->toolColorUnderlineLabel->Location = System::Drawing::Point(375, 545);
			   this->toolColorUnderlineLabel->Name = L"toolColorUnderlineLabel";
			   this->toolColorUnderlineLabel->Size = System::Drawing::Size(72, 13);
			   this->toolColorUnderlineLabel->TabIndex = 52;
			   this->toolColorUnderlineLabel->Text = L"************";
			   this->toolColorUnderlineLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			   this->newColorChangeDialog->AnyColor = true;
			   this->newColorChangeDialog->FullOpen = true;
			   this->newColorChangeDialog->SolidColorOnly = true;
			   this->toolColorChangeDialog->AnyColor = true;
			   this->toolColorChangeDialog->FullOpen = true;
			   this->toolColorChangeDialog->SolidColorOnly = true;
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			   this->BackColor = System::Drawing::SystemColors::ButtonShadow;
			   this->ClientSize = System::Drawing::Size(711, 694);
			   this->Controls->Add(this->toolColorUnderlineLabel);
			   this->Controls->Add(this->toolColorLabel);
			   this->Controls->Add(this->toolPickdiagButton);
			   this->Controls->Add(this->drawPenCheckbox);
			   this->Controls->Add(this->newPickToolCheckbox);
			   this->Controls->Add(this->searchPickToolCheckbox);
			   this->Controls->Add(this->colorPickToolCheckbox);
			   this->Controls->Add(this->colorPickerToolHexText);
			   this->Controls->Add(this->toolsLabel);
			   this->Controls->Add(this->pointerInfoText);
			   this->Controls->Add(this->aboveLabel);
			   this->Controls->Add(this->belowLabel);
			   this->Controls->Add(this->rangeAboveScroll);
			   this->Controls->Add(this->rangeBelowScroll);
			   this->Controls->Add(this->rangeTypeLabel);
			   this->Controls->Add(this->rangeDropdown);
			   this->Controls->Add(this->rangeMid);
			   this->Controls->Add(this->rangeBigLabel);
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
			   this->Controls->Add(this->ASMCheckBox);
			   this->Controls->Add(this->blurLabel);
			   this->Controls->Add(this->blurTrackbar);
			   this->Controls->Add(this->effectsLabel);
			   this->Controls->Add(this->brightnessLabel);
			   this->Controls->Add(this->brightnessTrackbar);
			   this->Controls->Add(this->averageTimeLabel);
			   this->Controls->Add(this->pictureBoxImg);
			   this->Controls->Add(this->mainBox);
			   this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			   this->MainMenuStrip = this->mainBox;
			   this->MinimumSize = System::Drawing::Size(634, 714);
			   this->Name = L"Form1";
			   this->Text = L"IMG Proc";
			   this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			   this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			   this->SizeChanged += gcnew System::EventHandler(this, &Form1::Form1_SizeChanged);
			   this->mainBox->ResumeLayout(false);
			   this->mainBox->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->brightnessTrackbar))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurTrackbar))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->rangeBelowScroll))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->rangeAboveScroll))->EndInit();
			   this->ResumeLayout(false);
			   this->PerformLayout();

		   }
	#pragma endregion
		private: System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
		{
			Application::Exit();
		}
		private: System::Void toolColorLabel_Click(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void mainBox_ItemClicked(System::Object^ sender, System::Windows::Forms::ToolStripItemClickedEventArgs^ e) {
		}
		private: System::Void openToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (dlgOpen->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				try
				{	
					temp = (Bitmap^)Image::FromFile(dlgOpen->FileName);
					
					if (!temp->PixelFormat.Equals(PixelFormat::Format24bppRgb) )
					{	
						MessageBox::Show("Image's bit depth should be 24, not " + temp->PixelFormat.ToString());
						return;
					}
					else 
					{
						SaveOriginalImage(temp);
						bmpFront = temp->Clone(imgRect,PixelFormat::Format24bppRgb);
						delete[] temp;
					}
					x_y = new unsigned int[3];
					x_y[0] = X;
					x_y[1] = Y;
					x_y[2] = onImage;

					String^ fullname = dlgOpen->SafeFileName;
					fileName = "";

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
					resetButton->Enabled = true;

					toolPickdiagButton->Enabled = true;
					searchColorButton->Enabled = true;
					newColorButton->Enabled = true;
					
					keepIMGCheckBox->Enabled = true;
					saveMenuItem->Enabled = true;
					negativeButton->Enabled = true;
					greyscaleButton->Enabled = true;
					rangeDropdown->Enabled = true;
					colorPickToolCheckbox->Enabled = true;
					searchPickToolCheckbox->Enabled = true;
					newPickToolCheckbox->Enabled = true;

					resetAll();

					boxW = pictureBoxImg->Width;
					boxH = pictureBoxImg->Height;

					realW = pictureBoxImg->Image->Width;
					realH = pictureBoxImg->Image->Height;

					hRatio = (double)boxH / (double)realH;
					wRatio = (double)boxW / (double)realW;

					dispW = (int)(realW * hRatio);
					whiteSpaceW = (int)(((float)(boxW - dispW)) / 2);

					dispH = (int)(realH * wRatio);
					whiteSpaceH = (int)(((float)(boxH - dispH)) / 2);

					minRatio = (hRatio < wRatio) ? 1.0/hRatio : 1.0/wRatio;

					ratioIndex = (hRatio < wRatio) ? 0 : 1;

					this->Text = fileName + " - IMG Proc";
					widthinbytes = realW * 3;
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

		private: System::Void resetButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			ResetIMG((unsigned char*)bmpData->Scan0.ToPointer(), bmpOriginal, imgSizeInBytes);
			bmpFront->UnlockBits(bmpData);

			pictureBoxImg->Image = bmpFront;
			
			resetAll();

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
				ASMGreyscale((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes);
				finishTime = clock();
				Greyscale_asmTotal += finishTime - startTime;
				Greyscale_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(Greyscale_asmTotal / Greyscale_asmCount, 2) + "ms";
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

		private: System::Void searchColorButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			unsigned char r, g, b;
			String^ name;
			if (searchColorChangeDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				sColor = searchColorChangeDialog->Color;
				r = sColor.R;
				g = sColor.G;
				b = sColor.B;
				if (!sColor.IsKnownColor) { name = sColor.Name->Substring(2); }
				else { name = sColor.Name; }

				Color foreclr = Color::FromArgb(255 - r, 255 - g, 255 - b);

				searchColorText->Text = name;
				searchColorText->ForeColor = foreclr;
				searchColorText->BackColor = sColor;

				rangeMid->BackColor = sColor;

				HSL[0] = (unsigned char)Math::Round(sColor.GetHue() * 0.6666666666);
				HSL[1] = (unsigned char)Math::Round(sColor.GetSaturation() * 240.0);
				HSL[2] = (unsigned char)Math::Round(sColor.GetBrightness() * 240.0);

				HSL_RANGES = new unsigned char[6]{ 0,0, 0,0, 0,0 };
				HSL_COLORS = new unsigned char[18]{ 0,0,0, 0,0,0,
													0,0,0, 0,0,0,
													0,0,0, 0,0,0 };

				if (rangeDropdown->SelectedIndex != -1)
				{
					rangeBelowScroll->Minimum = (short)HSL[rangeDropdown->SelectedIndex] * -1;
					rangeAboveScroll->Maximum = 240 - (short)HSL[rangeDropdown->SelectedIndex];
					rangeAboveScroll->Value = 0;
					rangeBelowScroll->Value = 0;
					belowLabel->Text = "-0";
					aboveLabel->Text = "+0";

					belowLabel->BackColor = Color::DarkCyan;
					belowLabel->ForeColor = Color::White;
					aboveLabel->BackColor = Color::DarkCyan;
					aboveLabel->ForeColor = Color::White;

					rangeMid->Text = HSL[rangeDropdown->SelectedIndex].ToString();
					rangeMid->ForeColor = foreclr;
				}

				if (!searchColorText->Text->Equals(String::Empty))
				{
					colorApplyButton->Enabled = true;
				}
			}
		}
		private: System::Void newColorButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			unsigned char r, g, b;
			String^ name;
			if (newColorChangeDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				nColor = newColorChangeDialog->Color;
				r = 255 - nColor.R;
				g = 255 - nColor.G;
				b = 255 - nColor.B;
				if (!nColor.IsKnownColor) { name = nColor.Name->Substring(2); }
				else { name = nColor.Name; }

				newColorText->Text = name;
				newColorText->ForeColor = Color::FromArgb(r, g, b);
				newColorText->BackColor = nColor;

				if (!newColorText->Text->Equals(String::Empty))
				{
					colorApplyButton->Enabled = true;
				}
			}
		}

		private: System::Void rangeBelowScroll_Scroll(System::Object^ sender, System::EventArgs^ e) 
		{
			belowLabel->Text = "-" + (rangeBelowScroll->Value*-1).ToString();

			temprgb = new unsigned char[3];
			temphsl = new unsigned char[3]{HSL[0],HSL[1] ,HSL[2] };
			temphsl[rangeDropdown->SelectedIndex] += rangeBelowScroll->Value;

			convert2RGB((float)temphsl[0], (float)temphsl[1], (float)temphsl[2], temprgb);

			belowLabel->BackColor = Color::FromArgb(temprgb[0], temprgb[1], temprgb[2]);
			belowLabel->ForeColor = Color::FromArgb(255-temprgb[0], 255-temprgb[1], 255-temprgb[2]);
			
			HSL_RANGES[rangeDropdown->SelectedIndex * 2] = rangeBelowScroll->Value * (-1);

			HSL_COLORS[rangeDropdown->SelectedIndex * 4] = temprgb[0];
			HSL_COLORS[rangeDropdown->SelectedIndex * 4 + 1] = temprgb[1];
			HSL_COLORS[rangeDropdown->SelectedIndex * 4 + 2] = temprgb[2];

		}
		private: System::Void rangeAboveScroll_Scroll(System::Object^ sender, System::EventArgs^ e) 
		{
			aboveLabel->Text = "+" + rangeAboveScroll->Value.ToString();

			temprgb = new unsigned char[3];
			temphsl = new unsigned char[3]{ HSL[0],HSL[1] ,HSL[2] };
			temphsl[rangeDropdown->SelectedIndex] += rangeAboveScroll->Value;

			convert2RGB((float)temphsl[0], (float)temphsl[1], (float)temphsl[2], temprgb);

			aboveLabel->BackColor = Color::FromArgb(temprgb[0], temprgb[1], temprgb[2]);
			aboveLabel->ForeColor = Color::FromArgb(255 - temprgb[0], 255 - temprgb[1], 255 - temprgb[2]);

			HSL_RANGES[rangeDropdown->SelectedIndex * 2 + 1] = rangeAboveScroll->Value;

			HSL_COLORS[rangeDropdown->SelectedIndex * 4 + 3] = temprgb[0];
			HSL_COLORS[rangeDropdown->SelectedIndex * 4 + 4] = temprgb[1];
			HSL_COLORS[rangeDropdown->SelectedIndex * 4 + 5] = temprgb[2];

		}

		private: System::Void rangeDropdown_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (rangeDropdown->SelectedIndex == -1)
			{
				rangeAboveScroll->Enabled = false;
				rangeBelowScroll->Enabled = false;
			}
			else
			{
				int index = rangeDropdown->SelectedIndex;
				rangeAboveScroll->Enabled = true;
				rangeBelowScroll->Enabled = true;
				colorApplyButton->Enabled = true;

				rangeBelowScroll->Minimum = (short)HSL[index] * -1;
				rangeAboveScroll->Maximum = 240 - (short)HSL[index];

				rangeAboveScroll->Value = HSL_RANGES[index * 2 + 1];
				rangeBelowScroll->Value = HSL_RANGES[index * 2]*(-1);
				belowLabel->Text = "-" + HSL_RANGES[index*2];
				aboveLabel->Text = "+" + HSL_RANGES[index * 2 + 1];

				belowLabel->BackColor = Color::FromArgb(HSL_COLORS[index*4], HSL_COLORS[index*4 +1], HSL_COLORS[index*4 +2]);
				belowLabel->ForeColor = Color::FromArgb(255-HSL_COLORS[index * 4], 255-HSL_COLORS[index * 4 + 1], 255-HSL_COLORS[index * 4 + 2]);
				aboveLabel->BackColor = Color::FromArgb(HSL_COLORS[index * 4 + 3], HSL_COLORS[index * 4 + 4], HSL_COLORS[index * 4 + 5]);
				aboveLabel->ForeColor = Color::FromArgb(255-HSL_COLORS[index * 4 + 3],255- HSL_COLORS[index * 4 + 4], 255-HSL_COLORS[index * 4 + 5]);

				rangeMid->BackColor = searchColorText->BackColor;
				rangeMid->Text = HSL[index].ToString();
				rangeMid->ForeColor = searchColorText->ForeColor;
			}
		}

		private: System::Void colorApplyButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (searchColorText->Text->Equals(String::Empty) || newColorText->Text->Equals(String::Empty))
			{
				return;
			}

			bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
			long startTime, finishTime;

			if (keepLast) {
				Copy((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG);
			}
			else {
				Copy(bmpOriginal, keptIMG);
				ResetIMG((unsigned char*)bmpData->Scan0.ToPointer(), keptIMG, imgSizeInBytes);
			}

			if (ASMCheckBox->Checked)
			{	
				HSL_RANGES_temp = new unsigned char[6];
				HSL_RANGES_temp[0] = HSL[0] - HSL_RANGES[0];
				HSL_RANGES_temp[1] = HSL[0] + HSL_RANGES[1];
				HSL_RANGES_temp[2] = HSL[1] - HSL_RANGES[2];
				HSL_RANGES_temp[3] = HSL[1] + HSL_RANGES[3];
				HSL_RANGES_temp[4] = HSL[2] - HSL_RANGES[4];
				HSL_RANGES_temp[5] = HSL[2] + HSL_RANGES[5];

				temp_nrgb = new unsigned char[3];
				temp_nrgb[0] = (unsigned char)nColor.R;
				temp_nrgb[1] = (unsigned char)nColor.G;
				temp_nrgb[2] = (unsigned char)nColor.B;

				startTime = clock();
				ASMColorChangeColorRange((unsigned char*)bmpData->Scan0.ToPointer(),temp_nrgb,HSL_RANGES_temp,imgSizeInBytes);
				finishTime = clock();
				ColorChange_asmTotal += finishTime - startTime;
				ColorChange_asmCount++;
				averageTimeLabel->Text = "Average using ASM: " + Math::Round(ColorChange_asmTotal / ColorChange_asmCount, 2) + "ms";
			}
			else
			{	
				HSL_RANGES_temp = new unsigned char[6];
				HSL_RANGES_temp[0] = HSL[0] - HSL_RANGES[0];
				HSL_RANGES_temp[1] = HSL[0] + HSL_RANGES[1];
				HSL_RANGES_temp[2] = HSL[1] - HSL_RANGES[2];
				HSL_RANGES_temp[3] = HSL[1] + HSL_RANGES[3];
				HSL_RANGES_temp[4] = HSL[2] - HSL_RANGES[4];
				HSL_RANGES_temp[5] = HSL[2] + HSL_RANGES[5];

				startTime = clock();
				CPPColorChangeColorRange((unsigned char*)bmpData->Scan0.ToPointer(), nColor, HSL_RANGES_temp, imgSizeInBytes);
				finishTime = clock();
				ColorChange_cppTotal += finishTime - startTime;
				ColorChange_cppCount++;
				averageTimeLabel->Text = "Average using C++: " + Math::Round(ColorChange_cppTotal / ColorChange_cppCount, 2) + "ms";

			}
			bmpFront->UnlockBits(bmpData);
			pictureBoxImg->Image = bmpFront;
			lastAction = "_color_range_changed";
		}

		private: System::Void pictureBoxImg_MouseMove(System::Object^ sender, MouseEventArgs^ e)
		{	
			
			if (bmpFront != nullptr) 
			{
				x_y[0] = e->Location.X;
				x_y[1] = e->Location.Y;
				x_y[2] = onImage;

				if (ASMCheckBox->Checked)
				{
					startMove = clock();
					ASMCoordinates(x_y, ratioIndex,
						whiteSpaceW, whiteSpaceH,
						dispW, dispH,
						boxW, boxH);
				}
				else
				{
					startMove = clock();
					CPPCoordinates(x_y, ratioIndex,
						whiteSpaceW, whiteSpaceH,
						dispW, dispH,
						boxW, boxH);
				}

				X = x_y[0];
				Y = x_y[1];
				onImage = x_y[2];

				if (drawPenCheckbox->Checked && e->Button.Equals(leftbutton) && onImage)
				{	
					lastAction = "_painted";
					bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);
					scan0 = (unsigned char*)bmpData->Scan0.ToPointer();

					change_index = (widthinbytes * ((int)(minRatio * (double)(Y-1))) + 3 * ((int)(minRatio * (double)(X-1))));
					
					//4px coloring, right bottom change, rest add

					//top
					//	left , lum highest
					scan0[change_index] = rgb_pickertool[2];
					scan0[change_index + 1] = rgb_pickertool[1];
					scan0[change_index + 2] = rgb_pickertool[0];
					
					if (X != boxW)
					{
						//	right , lum higher
						scan0[change_index + 3] = rgb_pickertool[2];
						scan0[change_index + 4] = rgb_pickertool[1];
						scan0[change_index + 5] = rgb_pickertool[0];

					}

					//bottom

					if (Y != boxH)
					{
						//	right , exact pixel
						change_index += widthinbytes;
						scan0[change_index] = rgb_pickertool[2];
						scan0[change_index + 1] = rgb_pickertool[1];
						scan0[change_index + 2] = rgb_pickertool[0];
						if (X != boxW)
						{
							//	left ,  lum higher
							scan0[change_index - 3] = rgb_pickertool[2];
							scan0[change_index - 2] = rgb_pickertool[1]; 
							scan0[change_index - 1] = rgb_pickertool[0];
						}
					}

					bmpFront->UnlockBits(bmpData);
					pictureBoxImg->Image = bmpFront;
				}
				else
				{
					pointerInfoText->Text = "Pos:("+ X + "," + Y + ")" + " - Real:(" + (Math::Round(minRatio * (double)X)) + "," + (Math::Round(minRatio * (double)Y)) + ")";
				}

				finishMove = clock();

				if (ASMCheckBox->Checked)
				{
					Move_asmCount++;
					Move_asmTotal += finishMove - startMove;
					averageTimeLabel->Text = "Average using ASM: " + Math::Round(Move_asmTotal / Move_asmCount, 2) + "ms";
				}
				else
				{
					Move_cppCount++;
					Move_cppTotal += finishMove - startMove;
					averageTimeLabel->Text = "Average using C++: " + Math::Round(Move_cppTotal / Move_cppCount, 2) + "ms";
				}
			}
			
		}

		private: System::Void Form1_SizeChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			if (bmpFront != nullptr)
			{
				boxW = pictureBoxImg->Width;
				boxH = pictureBoxImg->Height;

				realW = pictureBoxImg->Image->Width;
				realH = pictureBoxImg->Image->Height;

				hRatio = (double)boxH / (double)realH;
				wRatio = (double)boxW / (double)realW;

				dispW = (int)(realW * hRatio);
				whiteSpaceW = (int)(((float)(boxW - dispW)) / 2);

				dispH = (int)(realH * wRatio);
				whiteSpaceH = (int)(((float)(boxH - dispH)) / 2);

				minRatio = (hRatio < wRatio) ? 1.0/hRatio : 1.0/wRatio;

				ratioIndex = (hRatio < wRatio) ? 0 : 1;
			}
		}

		private: System::Void pictureBoxImg_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			if (usingPickerPointer && X != 0 && Y != 0)
			{	
				
				if (pickerIndex == 0) 
				{	
					rgb_pickertool = new unsigned char[3];
					pickedPx = bmpFront->GetPixel((int)(minRatio * (double)(X))-1, (int)(minRatio * (double)(Y))-1);
					
					rgb_pickertool[0] = pickedPx.R;
					rgb_pickertool[1] = pickedPx.G;
					rgb_pickertool[2] = pickedPx.B;

					if (!pickedPx.IsKnownColor) { pickedColorName = pickedPx.Name->Substring(2); }
					else { pickedColorName = pickedPx.Name; }

					colorPickerToolHexText->Text = pickedColorName; 
					colorPickerToolHexText->ForeColor = Color::FromArgb(255 - rgb_pickertool[0], 255 - rgb_pickertool[1], 255 - rgb_pickertool[2]);
					colorPickerToolHexText->BackColor = pickedPx;
				
					drawPen = gcnew Pen(pickedPx);
					drawPenCheckbox->Enabled = true;
				}
				// search color picked
				else if (pickerIndex == 1) 
				{  
					sColor = bmpFront->GetPixel((int)(minRatio * (double)(X)), (int)(minRatio * (double)(Y)));
					
					if (!sColor.IsKnownColor) { pickedColorName = sColor.Name->Substring(2); }
					else { pickedColorName = sColor.Name; }

					searchColorText->Text = pickedColorName;
					searchColorText->ForeColor = Color::FromArgb(255 - sColor.R, 255 - sColor.G, 255 - sColor.B);
					searchColorText->BackColor = sColor;

					rangeMid->BackColor = sColor;

					HSL[0] = (unsigned char)Math::Round(sColor.GetHue() * 0.6666666666);
					HSL[1] = (unsigned char)Math::Round(sColor.GetSaturation() * 240.0);
					HSL[2] = (unsigned char)Math::Round(sColor.GetBrightness() * 240.0);

					HSL_RANGES = new unsigned char[6]{ 0,0, 0,0, 0,0 };
					HSL_COLORS = new unsigned char[18]{ 0,0,0, 0,0,0,
														0,0,0, 0,0,0,
														0,0,0, 0,0,0 };

					// Changed color when range type was selected
					if (rangeDropdown->SelectedIndex != -1)
					{
						rangeBelowScroll->Minimum = (short)HSL[rangeDropdown->SelectedIndex] * -1;
						rangeAboveScroll->Maximum = 240 - (short)HSL[rangeDropdown->SelectedIndex];
						rangeAboveScroll->Value = 0;
						rangeBelowScroll->Value = 0;
						belowLabel->Text = "-0";
						aboveLabel->Text = "+0";

						belowLabel->BackColor = Color::DarkCyan;
						belowLabel->ForeColor = Color::White;
						aboveLabel->BackColor = Color::DarkCyan;
						aboveLabel->ForeColor = Color::White;

						rangeMid->Text = HSL[rangeDropdown->SelectedIndex].ToString();
						rangeMid->ForeColor = Color::FromArgb(255 - sColor.R, 255 - sColor.G, 255 - sColor.B);
					}

					if (!newColorText->Text->Equals(String::Empty))
					{
						colorApplyButton->Enabled = true;
					}
				}
				// new color picked
				else if (pickerIndex == 2) 
				{ 
					nColor = bmpFront->GetPixel((int)(minRatio * (double)(X)), (int)(minRatio * (double)(Y)));

					newColorText->Text = pickedColorName;
					newColorText->ForeColor = Color::FromArgb(255 - nColor.R, 255 - nColor.G, 255 - nColor.B);
					newColorText->BackColor = nColor;
					if (!searchColorText->Text->Equals(String::Empty))
					{
						colorApplyButton->Enabled = true;
					}
				}
			}
		}

		private: System::Void colorPickToolCheckbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (colorPickToolCheckbox->Checked)
			{
				searchPickToolCheckbox->Checked = false;
				newPickToolCheckbox->Checked = false;
				drawPenCheckbox->Checked = false;

				this->pictureBoxImg->Cursor = gcnew System::Windows::Forms::Cursor("picker.ico");

				usingPickerPointer = true;
				pickerIndex = 0;
			}
			else
			{	
				this->pictureBoxImg->Cursor = System::Windows::Forms::Cursors::Cross;
				usingPickerPointer = false;
				pickerIndex = -1;
			}
		}

		private: System::Void searchPickToolCheckbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (searchPickToolCheckbox->Checked)
			{
				colorPickToolCheckbox->Checked = false;
				newPickToolCheckbox->Checked = false;
				drawPenCheckbox->Checked = false;

				this->pictureBoxImg->Cursor = gcnew System::Windows::Forms::Cursor("picker.ico");

				usingPickerPointer = true;
				pickerIndex = 1;
			}
			else
			{
				this->pictureBoxImg->Cursor = System::Windows::Forms::Cursors::Cross;
				usingPickerPointer = false;
				pickerIndex = -1;
			}
		}

		private: System::Void newPickToolCheckbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (newPickToolCheckbox->Checked)
			{	
				colorPickToolCheckbox->Checked = false;
				searchPickToolCheckbox->Checked = false;
				drawPenCheckbox->Checked = false;

				this->pictureBoxImg->Cursor = gcnew System::Windows::Forms::Cursor("picker.ico");

				usingPickerPointer = true;
				pickerIndex = 2;
			}
			else
			{
				this->pictureBoxImg->Cursor = System::Windows::Forms::Cursors::Cross;
				usingPickerPointer = false;
				pickerIndex = -1;
			}
		}

		private: System::Void toolPickdiagButton_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			if (toolColorChangeDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				pickedPx = toolColorChangeDialog->Color;
				if (!pickedPx.IsKnownColor) { pickedColorName = pickedPx.Name->Substring(2); }
				else { pickedColorName = pickedPx.Name; }

				rgb_pickertool = new unsigned char[3];

				rgb_pickertool[0] = pickedPx.R;
				rgb_pickertool[1] = pickedPx.G;
				rgb_pickertool[2] = pickedPx.B;

				colorPickerToolHexText->Text = pickedColorName;
				colorPickerToolHexText->ForeColor = Color::FromArgb(255 - rgb_pickertool[0], 255 - rgb_pickertool[1], 255 - rgb_pickertool[2]);
				colorPickerToolHexText->BackColor = pickedPx;

				drawPenCheckbox->Enabled = true;
			}
			
		}

		private: System::Void drawPenCheckbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			if (drawPenCheckbox->Checked)
			{	
				colorPickToolCheckbox->Checked = false;
				searchPickToolCheckbox->Checked = false;
				newPickToolCheckbox->Checked = false;

				this->pictureBoxImg->Cursor = gcnew System::Windows::Forms::Cursor("pen.ico");
			}
			else
			{
				this->pictureBoxImg->Cursor = System::Windows::Forms::Cursors::Cross;
			}
		}

	};
}
