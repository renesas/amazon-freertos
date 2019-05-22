using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Security.Cryptography;

namespace Renesas_Secure_Flash_Programmer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            comboBox1.Text = "RX65N(ROM 2048KB: Bootloader=128KB)";
            comboBox2.Text = "SHA1";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // Displays a OpenFileDialog so the user can save the Image
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Filter = "Motorola Format File|*.mot";
            openFileDialog1.Title = "Open the Motorola Format File";
            openFileDialog1.ShowDialog();
            if (openFileDialog1.FileName == "")
            {
                print_log("please specify the motorola file name.\r\n");
                return;
            }
            user_program_file_path.Text = openFileDialog1.FileName;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try {
                byte[] tmp = new byte[16];
                byte[] user_firm = new byte[16];
                byte[] user_firm_buf = new byte[16];
                uint current_user_firm_address = 0;
                byte[] UpProgram = new byte[16];
                byte[] checksum = new byte[16];
                byte[] iv = new byte[16];
                byte[] iv_init = new byte[16];
                byte[] tmpkey1 = new byte[16];
                byte[] tmpkey2 = new byte[16];
                byte[] SessionKey0 = new byte[16];
                byte[] SessionKey1 = new byte[16];
                byte[] code_flash_image = new byte[1024 * 1024 * 4];    //  4MB image
                byte[] data_flash_image = new byte[1024 * 32];          // 32KB image

                if (comboBox1.Text == "(select MCU)")
                {
                    print_log("please select MCU in settings.\r\n");
                    return;
                }

                // Displays a SaveFileDialog so the user can save
                SaveFileDialog saveFileDialog2 = new SaveFileDialog();
                saveFileDialog2.Filter = "Renesas Secure Update|*.rsu";
                saveFileDialog2.Title = "Save an Encrypted User Program File";
                saveFileDialog2.FileName = "userprog.rsu";

                if(DialogResult.OK != saveFileDialog2.ShowDialog())
                {
                    print_log("please specifiy the output file.\r\n");
                    return;
                }

                if (saveFileDialog2.FileName == "")
                {
                    print_log("please specify the output file name.\r\n");
                    return;
                }
                if (user_program_file_path.Text == "")
                {
                    print_log("please specify the motorola file name.\r\n");
                    return;
                }

                /* fill the code flash image with 0xff */
                for (int i = 0; i < code_flash_image.Length; i++)
                {
                    code_flash_image[i] = 0xff;
                }

                /* fill the data flash image with 0xff */
                for (int i = 0; i < data_flash_image.Length; i++)
                {
                    data_flash_image[i] = 0xff;
                }

                //StreamReaderの生成
                System.IO.StreamReader sr = new System.IO.StreamReader(user_program_file_path.Text);

                //StreamWriterの生成
                System.IO.StreamWriter sw = new System.IO.StreamWriter(saveFileDialog2.FileName);

                while (true)
                {
                    string line = sr.ReadLine();
                    if (line == null)
                    {
                        break;
                    }

                    string[] line_buf = new string[16];

                    line_buf[0] = line.Substring(0, 2);    // type field
                    line_buf[1] = line.Substring(2, 2);    // length

                    switch (line_buf[0])
                    {
                        case "S0":
                            line_buf[2] = line.Substring(4, 4);                 // zero
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S1":
                            line_buf[2] = line.Substring(4, 4);                 // address
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S2":
                            line_buf[2] = line.Substring(4, 6);                 // address
                            line_buf[3] = line.Substring(10, line.Length - 10); // comment
                            break;
                        case "S3":
                            line_buf[2] = line.Substring(4, 8);                 // address
                            line_buf[3] = line.Substring(12, line.Length - 12); // comment
                            break;
                        case "S4":
                            break;
                        case "S5":
                            line_buf[2] = line.Substring(4, 4);                 // recode number
                            break;
                        case "S6":
                            break;
                        case "S7":
                            break;
                    }

                    if ((line_buf[0] == "S3") || (line_buf[0] == "S2"))
                    {
                        int data_len;
                        if (line_buf[0] == "S3")
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 5;     // -5 means: (address = 4 byte + checksum = 1 byte)
                        }
                        else
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 4;     // -4 means: (address = 3 byte + checksum = 1 byte)
                        }

                        if (comboBox1.Text == "RX65N(ROM 2048KB: Bootloader=128KB)")
                        {
                            current_user_firm_address = Convert.ToUInt32(line_buf[2], 16);

                            if ((current_user_firm_address >= data_flash_top_address)
                                && (current_user_firm_address <= data_flash_bottom_address))
                            {
                                if ((current_user_firm_address < user_program_const_data_top_address)
                                    || (current_user_firm_address > user_program_const_data_bottom_address))
                                {
                                    print_log(String.Format("your motorola file includes prohibit address 0x{0:x08} on data flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_const_data_top_address, user_program_const_data_bottom_address));
                                    sw.Close();
                                    sr.Close();
                                    return;
                                }
                                uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_const_data_top_address;
                                for (int i = 0; (i / 2) < data_len; i += 2)
                                {
                                    data_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                                }
                                current_user_firm_address = 0;
                                continue;
                            }

                            if ((current_user_firm_address >= code_flash_top_address)
                                && (current_user_firm_address <= code_flash_bottom_address))
                            {
                                if ((current_user_firm_address < user_program_top_address)
                                    || (current_user_firm_address > user_program_bottom_address))
                                {
                                    print_log(String.Format("your motorola file includes prohibit address 0x{0:x08} on code flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_top_address, user_program_bottom_address));
                                    sw.Close();
                                    sr.Close();
                                    return;
                                }
                                uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_top_address;
                                for (int i = 0; (i / 2) < data_len; i += 2)
                                {
                                    code_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                                }

                                current_user_firm_address = 0;
                                continue;
                            }
                        }
                    }
                }

                // calculate hash
                byte[] bs;
                string hash_value;

                if (comboBox1.Text != "SHA1")
                {
                    System.Security.Cryptography.SHA1CryptoServiceProvider sha_1 =
                        new System.Security.Cryptography.SHA1CryptoServiceProvider();
                    int offset = Convert.ToInt32(user_program_bottom_address - user_program_top_address);
                    bs = sha_1.ComputeHash(code_flash_image, 0, offset + 1);
                    sha_1.Clear();
                    hash_value = Convert.ToBase64String(bs, 0, 20);
                }
                else
                {
                    print_log(String.Format("selected illegal hash type"));
                    sw.Close();
                    sr.Close();
                    return;
                }

                string hash_string;
                if (comboBox1.Text != "SHA1")
                {
                    hash_string = "sha1 ";
                }
                else
                {
                    print_log(String.Format("selected illegal hash type"));
                    sw.Close();
                    sr.Close();
                    return;
                }
                hash_string += hash_value;
                hash_string += "\r\n";
                sw.Write(hash_string);

                string script;

                for (int i = 0; i < (user_program_const_data_bottom_address - user_program_const_data_top_address) + 1; i += 16)
                {
                    string user_program_base64 = Convert.ToBase64String(data_flash_image, i, 16);

                    script = "upconst ";
                    script += Convert.ToString(user_program_const_data_top_address + i, 16);
                    script += " ";
                    script += user_program_base64;
                    script += "\r\n";
                    sw.Write(script);
                }

                for (int i = 0; i < (user_program_bottom_address - user_program_top_address) + 1; i += 16)
                {
                    string user_program_base64 = Convert.ToBase64String(code_flash_image, i, 16);

                    script = "upprogram ";
                    script += Convert.ToString(user_program_top_address + i, 16);
                    script += " ";
                    script += user_program_base64;
                    script += "\r\n";
                    sw.Write(script);
                }
                sw.Close();
                sr.Close();
                print_log("generate succeeded.\r\n");
            }
            catch
            {
                print_log("exception has occurred.\r\n");
            }
        }

        private void print_log(string str)
        {
            info.Text += Convert.ToString(log_count);
            info.Text += ": ";
            info.Text += str;
            info.SelectionStart = info.Text.Length;
            info.Focus();
            info.ScrollToCaret();
            log_count++;
        }
        private int log_count = 0;

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(comboBox1.Text == "RX65N(ROM 2048KB: Bootloader=128KB)")
            {
                code_flash_top_address = 0xffe00000;
                code_flash_bottom_address = 0xffffffff;
                data_flash_top_address = 0x00100000;
                data_flash_bottom_address = 0x00107fff;
                user_program_top_address =    0xfff00000;
                user_program_bottom_address = 0xfffdffff;
                user_program_mirror_top_address = 0xffe00000;
                user_program_mirror_bottom_address = 0xffedffff;
                user_program_const_data_top_address = 0x00100200;
                user_program_const_data_bottom_address = 0x00107fff;
            }
        }
        private uint code_flash_top_address;
        private uint code_flash_bottom_address;
        private uint data_flash_top_address;
        private uint data_flash_bottom_address;
        private uint user_program_top_address;
        private uint user_program_bottom_address;
        private uint user_program_mirror_top_address;
        private uint user_program_mirror_bottom_address;
        private uint user_program_const_data_top_address;
        private uint user_program_const_data_bottom_address;
    }
}
