using System.Text;
using System.Windows;
using System.Net.Mail;
using System.Net;

namespace G2PatchMaker
{
	/// <summary>
	/// Interaction logic for SendMail.xaml
	/// </summary>
	public partial class SendMail : Window
	{
		public SendMail()
		{
			this.InitializeComponent();
		}

        private void send_Click(object sender, RoutedEventArgs e)
        {
            SmtpClient client = new SmtpClient("smtp.gmail.com", 587);
            NetworkCredential nc = new NetworkCredential("ssey1123@imc.co.kr", "tjsduddl1");
            client.UseDefaultCredentials = false;
            client.Credentials = nc;
            client.EnableSsl = true;

            MailAddress from = new MailAddress("ssey1123@imc.co.kr");
            MailAddress to = new MailAddress("ssey1123@imc.co.kr");
            MailMessage mail = new MailMessage(from, to);
            mail.Subject = "PatchMaker : " + mail_header.Text;
            mail.Body = mail_body.Text;
            mail.BodyEncoding = Encoding.UTF8;

            try
            {
                client.Send(mail);
                mail.Dispose();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            this.Close();
        }

        private void cancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
	}
}