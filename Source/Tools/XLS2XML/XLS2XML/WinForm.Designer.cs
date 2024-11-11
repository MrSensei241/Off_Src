namespace XLS2XML
{
    partial class WinForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.OpenFile = new System.Windows.Forms.Button();
            this.OpenText = new System.Windows.Forms.TextBox();
            this.SaveText = new System.Windows.Forms.TextBox();
            this.SaveFolder = new System.Windows.Forms.Button();
            this.SheetList = new System.Windows.Forms.ListBox();
            this.Convert = new System.Windows.Forms.Button();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // OpenFile
            // 
            this.OpenFile.Location = new System.Drawing.Point(188, 18);
            this.OpenFile.Name = "OpenFile";
            this.OpenFile.Size = new System.Drawing.Size(91, 24);
            this.OpenFile.TabIndex = 0;
            this.OpenFile.Text = "열기(O)";
            this.OpenFile.UseVisualStyleBackColor = true;
            this.OpenFile.Click += new System.EventHandler(this.OpenFile_Click);
            // 
            // OpenText
            // 
            this.OpenText.Location = new System.Drawing.Point(18, 19);
            this.OpenText.Name = "OpenText";
            this.OpenText.Size = new System.Drawing.Size(160, 21);
            this.OpenText.TabIndex = 1;
            // 
            // SaveText
            // 
            this.SaveText.Location = new System.Drawing.Point(18, 51);
            this.SaveText.Name = "SaveText";
            this.SaveText.Size = new System.Drawing.Size(160, 21);
            this.SaveText.TabIndex = 2;
            // 
            // SaveFolder
            // 
            this.SaveFolder.Location = new System.Drawing.Point(188, 49);
            this.SaveFolder.Name = "SaveFolder";
            this.SaveFolder.Size = new System.Drawing.Size(91, 24);
            this.SaveFolder.TabIndex = 3;
            this.SaveFolder.Text = "저장(S)";
            this.SaveFolder.UseVisualStyleBackColor = true;
            this.SaveFolder.Click += new System.EventHandler(this.SaveFolder_Click);
            // 
            // SheetList
            // 
            this.SheetList.FormattingEnabled = true;
            this.SheetList.ItemHeight = 12;
            this.SheetList.Location = new System.Drawing.Point(18, 82);
            this.SheetList.Name = "SheetList";
            this.SheetList.Size = new System.Drawing.Size(260, 124);
            this.SheetList.TabIndex = 4;
            // 
            // Convert
            // 
            this.Convert.Location = new System.Drawing.Point(204, 215);
            this.Convert.Name = "Convert";
            this.Convert.Size = new System.Drawing.Size(74, 39);
            this.Convert.TabIndex = 5;
            this.Convert.Text = "변환(C)";
            this.Convert.UseVisualStyleBackColor = true;
            this.Convert.Click += new System.EventHandler(this.Convert_Click);
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(19, 237);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(179, 14);
            this.progressBar.TabIndex = 6;
            this.progressBar.Visible = false;
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Enabled = false;
            this.textBox1.Location = new System.Drawing.Point(19, 215);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(179, 14);
            this.textBox1.TabIndex = 7;
            this.textBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // WinForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.Convert);
            this.Controls.Add(this.SheetList);
            this.Controls.Add(this.SaveFolder);
            this.Controls.Add(this.SaveText);
            this.Controls.Add(this.OpenText);
            this.Controls.Add(this.OpenFile);
            this.Name = "WinForm";
            this.Text = "WinForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button OpenFile;
        private System.Windows.Forms.TextBox OpenText;
        private System.Windows.Forms.TextBox SaveText;
        private System.Windows.Forms.Button SaveFolder;
        private System.Windows.Forms.ListBox SheetList;
        private System.Windows.Forms.Button Convert;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.TextBox textBox1;

    }
}