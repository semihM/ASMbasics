#pragma once

namespace CppCLRWinformsProjekt {

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
		static int imgSizeInBytes = -1;
		static Rectangle imgRect;
		BitmapData^ bmpData;

		static double cppCount = 0.0;
		static double cppTotal = 0.0;
		static double asmCount = 0.0;
		static double asmTotal = 0.0;

		void AdjustBrightness(unsigned char* bmp, short amount)
		{
			for (int i = 0; i < imgSizeInBytes; i++)
			{
				if ((short)bmpOriginal[i] + amount < 0) bmp[i] = 0;
				else if ((short)bmpOriginal[i] + amount > 255) bmp[i] = 255;
				else bmp[i] = bmpOriginal[i] + amount;
			}
		}

		void ClearOriginalImage()
		{
			if (bmpOriginal != nullptr) { delete[] bmpOriginal; }
		}

		//Make a copy of the original image
		void SaveOriginalImage(System::Drawing::Bitmap ^bmp)
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
	private: System::Windows::Forms::TrackBar^ trackBar1;
	private: System::Windows::Forms::Label^ avgbright;

	private: System::Windows::Forms::OpenFileDialog^ dlgOpen;

	private: System::ComponentModel::Container ^components;

	private: System::Windows::Forms::Label^ ASMLabel;
	private: System::Windows::Forms::TrackBar^ ASMtrackbar;

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
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pictureBoxImg = (gcnew System::Windows::Forms::PictureBox());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->avgbright = (gcnew System::Windows::Forms::Label());
			this->dlgOpen = (gcnew System::Windows::Forms::OpenFileDialog());
			this->ASMLabel = (gcnew System::Windows::Forms::Label());
			this->ASMtrackbar = (gcnew System::Windows::Forms::TrackBar());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ASMtrackbar))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(785, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			this->menuStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &Form1::menuStrip1_ItemClicked);
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->openToolStripMenuItem,
					this->exitToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			this->openToolStripMenuItem->Text = L"&Open";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// pictureBoxImg
			// 
			this->pictureBoxImg->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBoxImg->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBoxImg->Location = System::Drawing::Point(12, 27);
			this->pictureBoxImg->Name = L"pictureBoxImg";
			this->pictureBoxImg->Size = System::Drawing::Size(759, 349);
			this->pictureBoxImg->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxImg->TabIndex = 1;
			this->pictureBoxImg->TabStop = false;
			// 
			// trackBar1
			// 
			this->trackBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->trackBar1->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->trackBar1->Enabled = false;
			this->trackBar1->Location = System::Drawing::Point(12, 475);
			this->trackBar1->Maximum = 255;
			this->trackBar1->Minimum = -255;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(759, 45);
			this->trackBar1->TabIndex = 2;
			this->trackBar1->TickFrequency = 16;
			this->trackBar1->Scroll += gcnew System::EventHandler(this, &Form1::trackBar1_Scroll);
			// 
			// avgbright
			// 
			this->avgbright->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->avgbright->AutoSize = true;
			this->avgbright->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->avgbright->Location = System::Drawing::Point(12, 459);
			this->avgbright->Name = L"avgbright";
			this->avgbright->Size = System::Drawing::Size(51, 13);
			this->avgbright->TabIndex = 3;
			this->avgbright->Text = L"C++ AVG";
			this->avgbright->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// dlgOpen
			// 
			this->dlgOpen->FileName = L"openFileDialog1";
			this->dlgOpen->Filter = L"JPEG|*.jpg|Bitmap|*.bmp|All Files|*.*";
			// 
			// ASMLabel
			// 
			this->ASMLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->ASMLabel->AutoSize = true;
			this->ASMLabel->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->ASMLabel->Location = System::Drawing::Point(12, 384);
			this->ASMLabel->Name = L"ASMLabel";
			this->ASMLabel->Size = System::Drawing::Size(55, 13);
			this->ASMLabel->TabIndex = 5;
			this->ASMLabel->Text = L"ASM AVG";
			// 
			// ASMtrackbar
			// 
			this->ASMtrackbar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ASMtrackbar->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->ASMtrackbar->Enabled = false;
			this->ASMtrackbar->Location = System::Drawing::Point(12, 400);
			this->ASMtrackbar->Maximum = 255;
			this->ASMtrackbar->Minimum = -255;
			this->ASMtrackbar->Name = L"ASMtrackbar";
			this->ASMtrackbar->Size = System::Drawing::Size(759, 45);
			this->ASMtrackbar->TabIndex = 4;
			this->ASMtrackbar->TickFrequency = 16;
			this->ASMtrackbar->Scroll += gcnew System::EventHandler(this, &Form1::ASMtrackbar_Scroll);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->ClientSize = System::Drawing::Size(785, 527);
			this->Controls->Add(this->ASMLabel);
			this->Controls->Add(this->ASMtrackbar);
			this->Controls->Add(this->avgbright);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->pictureBoxImg);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxImg))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ASMtrackbar))->EndInit();
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

				SaveOriginalImage(bmpFront);

				pictureBoxImg->Image = bmpFront;

				trackBar1->Enabled = true;
				ASMtrackbar->Enabled = true;

				cppTotal = 0.0;
				cppCount = 0.0;
				asmCount = 0.0;
				asmTotal = 0.0;

			}
			catch(...)
			{
				MessageBox::Show("File could not be opened!");
			}
		}
	}
	private: System::Void trackBar1_Scroll(System::Object^ sender, System::EventArgs^ e) 
	{
		bmpData = bmpFront->LockBits(imgRect,ImageLockMode::WriteOnly,PixelFormat::Format24bppRgb);

		long startTime = clock();

		AdjustBrightness((unsigned char*)bmpData->Scan0.ToPointer(), trackBar1->Value);

		long finishTime = clock();
		
		bmpFront->UnlockBits(bmpData);

		pictureBoxImg->Image = bmpFront;

		cppTotal += finishTime - startTime;
		cppCount++;

		avgbright->Text = "C++ AVG: " + Math::Round(cppTotal / cppCount,2);
	}
	private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) 
	{
		ClearOriginalImage();
	}

	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) 
	{

	}
	private: System::Void ASMtrackbar_Scroll(System::Object^ sender, System::EventArgs^ e) 
	{
		bmpData = bmpFront->LockBits(imgRect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);

		long startTime = clock();

		ASMAdjustBrightness((unsigned char*)bmpData->Scan0.ToPointer(),bmpOriginal, ASMtrackbar->Value,imgSizeInBytes);

		long finishTime = clock();

		bmpFront->UnlockBits(bmpData);

		pictureBoxImg->Image = bmpFront;

		asmTotal += finishTime - startTime;
		asmCount++;

		ASMLabel->Text = "ASM AVG: " + Math::Round(asmTotal / asmCount, 2);
	}
};
	}
