#pragma once
#include <cmath>
#include "WordAnalyzer.h"

namespace test1 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^ tabControl1;
	protected:
	private: System::Windows::Forms::TabPage^ Grafic_page;
	private: System::Windows::Forms::TabPage^ Sort_page;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::TextBox^ le_from;

	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::TextBox^ le_step;

	private: System::Windows::Forms::TextBox^ le_to;

	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ chart1;
	private: System::Windows::Forms::Button^ button1;

	// second part 
	private: System::Windows::Forms::TextBox^ tb_leng_arr;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::TextBox^ tb_from_length;
	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::DataGridView^ dgv_arr_source;
	private: System::Windows::Forms::Label^ label8;
	private: System::Windows::Forms::Label^ label7;
	private: System::Windows::Forms::TextBox^ tb_to_length;
	private: System::Windows::Forms::DataGridView^ dgv_arr_result;

	private: System::Windows::Forms::Label^ label9;
	private: System::Windows::Forms::Button^ btn_clear;
	private: System::Windows::Forms::Button^ btn_process;
	private: System::Windows::Forms::Button^ btn_create;

	// KR 2 
	private: System::Windows::Forms::TabPage^ tabPage1;
	private: System::Windows::Forms::RichTextBox^ rtb_result;
	private: System::Windows::Forms::RichTextBox^ rtb_source;
	private: System::Windows::Forms::Label^ label11;
	private: System::Windows::Forms::Label^ label10;
	private: System::Windows::Forms::Label^ label12;
	private: System::Windows::Forms::Button^ btn_found_word;

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->Grafic_page = (gcnew System::Windows::Forms::TabPage());
			this->chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->le_to = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->le_from = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->le_step = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->Sort_page = (gcnew System::Windows::Forms::TabPage());
			this->btn_clear = (gcnew System::Windows::Forms::Button());
			this->btn_process = (gcnew System::Windows::Forms::Button());
			this->btn_create = (gcnew System::Windows::Forms::Button());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->dgv_arr_result = (gcnew System::Windows::Forms::DataGridView());
			this->dgv_arr_source = (gcnew System::Windows::Forms::DataGridView());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->tb_to_length = (gcnew System::Windows::Forms::TextBox());
			this->tb_from_length = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tb_leng_arr = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->btn_found_word = (gcnew System::Windows::Forms::Button());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->rtb_result = (gcnew System::Windows::Forms::RichTextBox());
			this->rtb_source = (gcnew System::Windows::Forms::RichTextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->tabControl1->SuspendLayout();
			this->Grafic_page->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->Sort_page->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgv_arr_result))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgv_arr_source))->BeginInit();
			this->tabPage1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->Grafic_page);
			this->tabControl1->Controls->Add(this->Sort_page);
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Location = System::Drawing::Point(12, 12);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(510, 435);
			this->tabControl1->TabIndex = 0;
			// 
			// Grafic_page
			// 
			this->Grafic_page->Controls->Add(this->chart1);
			this->Grafic_page->Controls->Add(this->button1);
			this->Grafic_page->Controls->Add(this->le_to);
			this->Grafic_page->Controls->Add(this->label4);
			this->Grafic_page->Controls->Add(this->le_from);
			this->Grafic_page->Controls->Add(this->label3);
			this->Grafic_page->Controls->Add(this->le_step);
			this->Grafic_page->Controls->Add(this->label2);
			this->Grafic_page->Controls->Add(this->pictureBox1);
			this->Grafic_page->Controls->Add(this->label1);
			this->Grafic_page->Location = System::Drawing::Point(4, 22);
			this->Grafic_page->Name = L"Grafic_page";
			this->Grafic_page->Padding = System::Windows::Forms::Padding(3);
			this->Grafic_page->Size = System::Drawing::Size(502, 409);
			this->Grafic_page->TabIndex = 0;
			this->Grafic_page->Text = L"Graphic";
			this->Grafic_page->UseVisualStyleBackColor = true;
			// 
			// chart1
			// 
			chartArea1->Name = L"ChartArea1";
			this->chart1->ChartAreas->Add(chartArea1);
			legend1->Name = L"Legend1";
			this->chart1->Legends->Add(legend1);
			this->chart1->Location = System::Drawing::Point(6, 110);
			this->chart1->Name = L"chart1";
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series1->Legend = L"Legend1";
			series1->Name = L"Series1";
			this->chart1->Series->Add(series1);
			this->chart1->Size = System::Drawing::Size(490, 293);
			this->chart1->TabIndex = 9;
			this->chart1->Text = L"chart1";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(296, 81);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(138, 23);
			this->button1->TabIndex = 8;
			this->button1->Text = L"Build graph";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// le_to
			// 
			this->le_to->Location = System::Drawing::Point(440, 36);
			this->le_to->Name = L"le_to";
			this->le_to->Size = System::Drawing::Size(48, 20);
			this->le_to->TabIndex = 7;
			this->le_to->Text = L"5";
			this->le_to->TextChanged += gcnew System::EventHandler(this, &MainForm::le_to_TextChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(418, 39);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(16, 13);
			this->label4->TabIndex = 6;
			this->label4->Text = L"to";
			// 
			// le_from
			// 
			this->le_from->Location = System::Drawing::Point(363, 36);
			this->le_from->Name = L"le_from";
			this->le_from->Size = System::Drawing::Size(48, 20);
			this->le_from->TabIndex = 5;
			this->le_from->Text = L"-5";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(217, 39);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(140, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Interval of variation of x from";
			// 
			// le_step
			// 
			this->le_step->Location = System::Drawing::Point(305, 10);
			this->le_step->Name = L"le_step";
			this->le_step->Size = System::Drawing::Size(53, 20);
			this->le_step->TabIndex = 3;
			this->le_step->Text = L"0,01";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(217, 13);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(82, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Variation step x:";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(9, 29);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(189, 75);
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(71, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"given fnction:";
			this->label1->Click += gcnew System::EventHandler(this, &MainForm::label1_Click);
			// 
			// Sort_page
			// 
			this->Sort_page->Controls->Add(this->btn_clear);
			this->Sort_page->Controls->Add(this->btn_process);
			this->Sort_page->Controls->Add(this->btn_create);
			this->Sort_page->Controls->Add(this->label9);
			this->Sort_page->Controls->Add(this->dgv_arr_result);
			this->Sort_page->Controls->Add(this->dgv_arr_source);
			this->Sort_page->Controls->Add(this->label8);
			this->Sort_page->Controls->Add(this->label7);
			this->Sort_page->Controls->Add(this->tb_to_length);
			this->Sort_page->Controls->Add(this->tb_from_length);
			this->Sort_page->Controls->Add(this->label6);
			this->Sort_page->Controls->Add(this->tb_leng_arr);
			this->Sort_page->Controls->Add(this->label5);
			this->Sort_page->Location = System::Drawing::Point(4, 22);
			this->Sort_page->Name = L"Sort_page";
			this->Sort_page->Padding = System::Windows::Forms::Padding(3);
			this->Sort_page->Size = System::Drawing::Size(502, 409);
			this->Sort_page->TabIndex = 1;
			this->Sort_page->Text = L"Sort";
			this->Sort_page->UseVisualStyleBackColor = true;
			// 
			// btn_clear
			// 
			this->btn_clear->Location = System::Drawing::Point(196, 145);
			this->btn_clear->Name = L"btn_clear";
			this->btn_clear->Size = System::Drawing::Size(128, 23);
			this->btn_clear->TabIndex = 13;
			this->btn_clear->Text = L"clear data";
			this->btn_clear->UseVisualStyleBackColor = true;
			this->btn_clear->Click += gcnew System::EventHandler(this, &MainForm::btn_clear_Click);
			// 
			// btn_process
			// 
			this->btn_process->Location = System::Drawing::Point(196, 116);
			this->btn_process->Name = L"btn_process";
			this->btn_process->Size = System::Drawing::Size(128, 23);
			this->btn_process->TabIndex = 12;
			this->btn_process->Text = L"Process array";
			this->btn_process->UseVisualStyleBackColor = true;
			this->btn_process->Click += gcnew System::EventHandler(this, &MainForm::btn_process_Click);
			// 
			// btn_create
			// 
			this->btn_create->Location = System::Drawing::Point(196, 87);
			this->btn_create->Name = L"btn_create";
			this->btn_create->Size = System::Drawing::Size(128, 23);
			this->btn_create->TabIndex = 11;
			this->btn_create->Text = L"Create an array";
			this->btn_create->UseVisualStyleBackColor = true;
			this->btn_create->Click += gcnew System::EventHandler(this, &MainForm::btn_create_Click);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(327, 64);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(69, 13);
			this->label9->TabIndex = 10;
			this->label9->Text = L"Result array :";
			// 
			// dgv_arr_result
			// 
			this->dgv_arr_result->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dgv_arr_result->Location = System::Drawing::Point(330, 87);
			this->dgv_arr_result->Name = L"dgv_arr_result";
			this->dgv_arr_result->Size = System::Drawing::Size(166, 316);
			this->dgv_arr_result->TabIndex = 9;
			// 
			// dgv_arr_source
			// 
			this->dgv_arr_source->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dgv_arr_source->Location = System::Drawing::Point(23, 87);
			this->dgv_arr_source->Name = L"dgv_arr_source";
			this->dgv_arr_source->Size = System::Drawing::Size(166, 316);
			this->dgv_arr_source->TabIndex = 8;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(20, 64);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(73, 13);
			this->label8->TabIndex = 7;
			this->label8->Text = L"Source array :";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(201, 38);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(16, 13);
			this->label7->TabIndex = 5;
			this->label7->Text = L"to";
			// 
			// tb_to_length
			// 
			this->tb_to_length->Location = System::Drawing::Point(223, 35);
			this->tb_to_length->Name = L"tb_to_length";
			this->tb_to_length->Size = System::Drawing::Size(43, 20);
			this->tb_to_length->TabIndex = 4;
			this->tb_to_length->Text = L"99";
			// 
			// tb_from_length
			// 
			this->tb_from_length->Location = System::Drawing::Point(152, 35);
			this->tb_from_length->Name = L"tb_from_length";
			this->tb_from_length->Size = System::Drawing::Size(43, 20);
			this->tb_from_length->TabIndex = 3;
			this->tb_from_length->Text = L"-100";
			this->tb_from_length->TextChanged += gcnew System::EventHandler(this, &MainForm::textBox1_TextChanged);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(20, 38);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(126, 13);
			this->label6->TabIndex = 2;
			this->label6->Text = L"Array value interval from :";
			// 
			// tb_leng_arr
			// 
			this->tb_leng_arr->Location = System::Drawing::Point(195, 9);
			this->tb_leng_arr->Name = L"tb_leng_arr";
			this->tb_leng_arr->Size = System::Drawing::Size(42, 20);
			this->tb_leng_arr->TabIndex = 1;
			this->tb_leng_arr->Text = L"8";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(20, 12);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(169, 13);
			this->label5->TabIndex = 0;
			this->label5->Text = L"Length of a one-dimensional array:";
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->btn_found_word);
			this->tabPage1->Controls->Add(this->label12);
			this->tabPage1->Controls->Add(this->rtb_result);
			this->tabPage1->Controls->Add(this->rtb_source);
			this->tabPage1->Controls->Add(this->label11);
			this->tabPage1->Controls->Add(this->label10);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Size = System::Drawing::Size(502, 409);
			this->tabPage1->TabIndex = 2;
			this->tabPage1->Text = L"KR 2";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// btn_found_word
			// 
			this->btn_found_word->Location = System::Drawing::Point(220, 373);
			this->btn_found_word->Name = L"btn_found_word";
			this->btn_found_word->Size = System::Drawing::Size(76, 23);
			this->btn_found_word->TabIndex = 5;
			this->btn_found_word->Text = L"Found word";
			this->btn_found_word->UseVisualStyleBackColor = true;
			this->btn_found_word->Click += gcnew System::EventHandler(this, &MainForm::btn_found_word_Click);
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(299, 37);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(66, 13);
			this->label12->TabIndex = 4;
			this->label12->Text = L"Found taskt:";
			// 
			// rtb_result
			// 
			this->rtb_result->Location = System::Drawing::Point(302, 62);
			this->rtb_result->Name = L"rtb_result";
			this->rtb_result->Size = System::Drawing::Size(197, 334);
			this->rtb_result->TabIndex = 3;
			this->rtb_result->Text = L"";
			// 
			// rtb_source
			// 
			this->rtb_source->Location = System::Drawing::Point(17, 62);
			this->rtb_source->Name = L"rtb_source";
			this->rtb_source->Size = System::Drawing::Size(197, 334);
			this->rtb_source->TabIndex = 2;
			this->rtb_source->Text = L"";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(14, 37);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(64, 13);
			this->label11->TabIndex = 1;
			this->label11->Text = L"Source text:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(14, 15);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(320, 13);
			this->label10->TabIndex = 0;
			this->label10->Text = L"Task: Find all the words in the text that end with a consonant letter";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(534, 459);
			this->Controls->Add(this->tabControl1);
			this->Name = L"MainForm";
			this->Text = L"Контрольная работа С++ (вариант 2)";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->tabControl1->ResumeLayout(false);
			this->Grafic_page->ResumeLayout(false);
			this->Grafic_page->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->Sort_page->ResumeLayout(false);
			this->Sort_page->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgv_arr_result))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgv_arr_source))->EndInit();
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void le_to_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		double step = Convert::ToDouble(le_step->Text);
		double minIntervalValue = Convert::ToDouble(le_from->Text);
		double maxIntervalValue = Convert::ToDouble(le_to->Text);
		
		chart1->Series->Clear(); 
		chart1->Series->Add("part_one");
		chart1->Series["part_one"]->ChartType =	System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
		chart1->Series->Add("part_two"); 
		chart1->Series["part_one"]->ChartType =	System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
		chart1->Series->Add("part_three");
		chart1->Series["part_three"]->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
		double y; 
		
		for (double x = minIntervalValue; x <=	maxIntervalValue; x = x + step) {
		
			if (abs(x) <= 1.0) {
				y = x * x;
				chart1->Series["part_one"]->Points->AddXY(x, y);
			}
			if (abs(x) > 1) {
				y = -1.0 / x;
				chart1->Series["part_two"]->Points-> AddXY(x, y);
			}		
		}
    }

	private: System::Void textBox1_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	
	private: int ArrayLength;
	private: double minValue;
	private: double maxValue;
	private: int* mainArray;
	
	private: System::Void btn_create_Click(System::Object^ sender, System::EventArgs^ e) {
		ArrayLength = Convert::ToInt32(tb_leng_arr->Text);
		minValue = Convert::ToInt32(tb_from_length->Text);
		maxValue = Convert::ToInt32(tb_to_length->Text);
	
		mainArray = new int[ArrayLength];
	
		Random^ r = gcnew Random;
		for (int i = 0; i < ArrayLength; i++) {
			mainArray[i] = Convert::ToInt32(minValue + Convert::ToDouble(r->Next(1, 100)) / 100.0 * Convert::ToDouble(System::Math::Abs(minValue - maxValue)));
		}
	
		dgv_arr_source->Columns->Add("Values", "Values");
		for (int j = 0; j < ArrayLength; j++) {
			dgv_arr_source->Rows->Add();
			dgv_arr_source->Rows[j]->Cells[0]->Value = mainArray[j];
		}
	}
	
	private: System::Void btn_process_Click(System::Object^ sender, System::EventArgs^ e) {
		SelectionSort(mainArray, ArrayLength);
	
		dgv_arr_result->Columns->Add("Values", "Values");
		for (int j = 0; j < ArrayLength; j++) {
			dgv_arr_result->Rows->Add();
			dgv_arr_result->Rows[j]->Cells[0]->Value = mainArray[j];
		}
	}
	
	private: System::Void Swap(int* e1, int* e2) {
		*e1 ^= *e2;
		*e2 ^= *e1;
		*e1 ^= *e2;
	}
	
	private: System::Void SelectionSort(int Arr[], int N) {
		for (int i = 0; i < N - 1; i++) {
			int minIndex = i;
			for (int j = i + 1; j < N; j++) {
				if (Arr[j] < Arr[minIndex]) {
					minIndex = j;
				}
			}
	
			if (minIndex != i) {
				Swap(&Arr[i], &Arr[minIndex]);
			}
		}
	}

	private: System::Void btn_clear_Click(System::Object^ sender, System::EventArgs^ e) {
		dgv_arr_source->Columns->Clear();
		dgv_arr_result->Columns->Clear();
	}

	private: System::Void btn_found_word_Click(System::Object^ sender, System::EventArgs^ e) {
		if (String::IsNullOrWhiteSpace(rtb_source->Text)) {
			MessageBox::Show(L"Введите текст для анализа!", L"Ошибка",
				MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		WordAnalyzer^ wordAnalyzer = gcnew WordAnalyzer();

		wordAnalyzer->setTextForAnalyze(rtb_source->Text);

		rtb_result->Text = wordAnalyzer->getWordsEndingWithConsonant();
	}
};
}
