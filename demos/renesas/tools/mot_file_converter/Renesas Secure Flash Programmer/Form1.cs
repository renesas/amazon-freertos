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
                uint pre_user_firm_address = 0;
                uint top_user_firm_address = 0;
                uint user_program_top_address_first_appear_flag = 0;
                byte[] UpProgram = new byte[16];
                byte[] checksum = new byte[16];
                byte[] iv = new byte[16];
                byte[] iv_init = new byte[16];
                byte[] tmpkey1 = new byte[16];
                byte[] tmpkey2 = new byte[16];
                byte[] SessionKey0 = new byte[16];
                byte[] SessionKey1 = new byte[16];
                byte[] image = new byte[1024 * 1024 * 4];  // 4MB image

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

                //ユーザプログラム鍵データ文字列をバイナリに変換する
                byte[] user_program_key = new byte[16];
                string[] block_buf = new string[32];


                for (int i = 0; i < image.Length; i++)
                {
                    image[i] = 0xff;
                }

                //AesCryptoServiceProviderオブジェクトの作成
                System.Security.Cryptography.AesCryptoServiceProvider
                aes = new System.Security.Cryptography.AesCryptoServiceProvider();

                //AESはブロックサイズ128bit、キー長128bit
                aes.BlockSize = 128; //ブロックサイズ
                aes.KeySize = 128; //キー長
                aes.Mode = System.Security.Cryptography.CipherMode.ECB; //ECBモード
                aes.Padding = PaddingMode.None;

                RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
                rng.GetBytes(iv);
                rng.GetBytes(tmpkey1);
                rng.GetBytes(tmpkey2);


                //AES暗号化オブジェクトの作成
                aes.Key = tmpkey1;
                System.Security.Cryptography.ICryptoTransform encrypt1 = aes.CreateEncryptor();
                aes.Key = tmpkey2;
                System.Security.Cryptography.ICryptoTransform encrypt2 = aes.CreateEncryptor();
                aes.Key = user_program_key;
                System.Security.Cryptography.ICryptoTransform encrypt3 = aes.CreateEncryptor();
                aes.Key = tmpkey1;
                System.Security.Cryptography.ICryptoTransform encrypt4 = aes.CreateEncryptor();

                //MemoryStreamの生成
                System.IO.MemoryStream ms1 = new System.IO.MemoryStream();
                System.IO.MemoryStream ms2 = new System.IO.MemoryStream();
                System.IO.MemoryStream ms3 = new System.IO.MemoryStream();
                System.IO.MemoryStream ms4 = new System.IO.MemoryStream();

                //CryptoStreamの作成
                System.Security.Cryptography.CryptoStream
                cs1 = new System.Security.Cryptography.CryptoStream(
                  ms1, encrypt1, System.Security.Cryptography.CryptoStreamMode.Write);
                System.Security.Cryptography.CryptoStream
                cs2 = new System.Security.Cryptography.CryptoStream(
                  ms2, encrypt2, System.Security.Cryptography.CryptoStreamMode.Write);
                System.Security.Cryptography.CryptoStream
                cs3 = new System.Security.Cryptography.CryptoStream(
                  ms3, encrypt3, System.Security.Cryptography.CryptoStreamMode.Write);
                System.Security.Cryptography.CryptoStream
                cs4 = new System.Security.Cryptography.CryptoStream(
                  ms4, encrypt4, System.Security.Cryptography.CryptoStreamMode.Write);

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

                    if (line_buf[0] == "S3")
                    {
                        int data_len = Convert.ToByte(line_buf[1], 16) - 5;     // -5 means: (address = 4 byte + checksum = 1 byte)

                        if (comboBox1.Text == "RX65N(ROM 2048KB: Bootloader=128KB)")
                        {

                            if (top_user_firm_address == 0)
                            {
                                top_user_firm_address = Convert.ToUInt32(line_buf[2], 16);
                                pre_user_firm_address = top_user_firm_address;

                                if (top_user_firm_address < user_program_top_address)
                                {

                                    if ((user_program_error_top_address <= pre_user_firm_address)
                                        && (pre_user_firm_address <= user_program_error_bottom_address))
                                    {
                                        print_log("mismatch motorola file top address and selected MCU top address.\r\n");
                                        sw.Close();
                                        sr.Close();
                                        return;
                                    }
                                    top_user_firm_address = 0;
                                    continue;
                                }
                            }
                        }


                        else
                        {
                            /* skip until user_program_top_address first appears */
                            if (user_program_top_address_first_appear_flag == 0)
                            {
                                if (user_program_top_address != Convert.ToUInt32(line_buf[2], 16))
                                {
                                    continue;
                                }
                                else
                                {
                                    user_program_top_address_first_appear_flag = 1;
                                    top_user_firm_address = Convert.ToUInt32(line_buf[2], 16);
                                    pre_user_firm_address = top_user_firm_address;
                                }
                            }
                        }

                        uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_top_address;
                        for (int i = 0; (i / 2) < data_len; i += 2)
                        {
                            image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                        }
                    }
                }

                int debugfileoutput = 0;
                if (debugfileoutput == 1)
                {
                    //StreamDebug(bin)の生成
                    System.IO.BinaryWriter sdb = new System.IO.BinaryWriter(File.Open("Debugout.bin", FileMode.Create));
                    for (int j = 0; j < user_program_bottom_address - user_program_top_address; j++)
                    {
                        sdb.Write(image[j]);
                    }
                    sdb.Close();

                    //byte型配列を16進数の文字列に変換
                    System.Text.StringBuilder result3 = new System.Text.StringBuilder();
                    foreach (byte b in image)
                    {
                        result3.Append(b.ToString("x2"));
                    }

                    char[] result4 = new char[(32) * 65536];
                    for (int j = 0; j < result3.Length; j++)
                    {
                        result4[j] = Convert.ToChar(result3[j]);
                    }
                }

                /* TSIP手順に従った暗号化: 今は無効化 */
                for (int i = 0; i < user_program_bottom_address - user_program_top_address; i += (aes.BlockSize / 8))
                {
                    for (int j = 0; j < aes.BlockSize / 8; j++)
                    {
//                        checksum[j] = Convert.ToByte(image[i + j] ^ checksum[j]);
                        UpProgram[j] = Convert.ToByte(image[i + j] ^ iv[j]);
                    }
                    for (int j = 0; j < aes.BlockSize / 8; j++)
                    {
                        cs2.Write(checksum, j, 1);  // encrypt using tmpkey2
                    }
                    tmp = ms2.GetBuffer();
                    for (int j = 0; j < aes.BlockSize / 8; j++)
                    {
//                        checksum[j] = tmp[i + j];
                    }
                    for (int j = 0; j < aes.BlockSize / 8; j++)
                    {
                        cs1.Write(UpProgram, j, 1);  // encrypt using tmpkey1
                    }
                    tmp = ms1.GetBuffer();
                    for (int j = 0; j < aes.BlockSize / 8; j++)
                    {
                        UpProgram[j] = tmp[i + j];
                    }
                }
                for (int i = 0; i < aes.BlockSize / 8; i++)
                {
//                    checksum[i] = Convert.ToByte(iv[i] ^ checksum[i]);
                }
                cs4.Write(checksum, 0, aes.BlockSize / 8);  // encrypt using tmpkey1
                tmp = ms4.GetBuffer();
                for (int i = 0; i < aes.BlockSize / 8; i++)
                {
//                    checksum[i] = tmp[i];
                }

                //ハッシュ値を計算する
                byte[] bs;
                string hashvalue;

                if (comboBox1.Text != "SHA1")
                {
                        /* ハッシュ計算 */
                        System.Security.Cryptography.SHA1CryptoServiceProvider sha_1 =
                       new System.Security.Cryptography.SHA1CryptoServiceProvider();
                    //ハッシュ値を計算する
                    int offset = Convert.ToInt32(user_program_bottom_address - user_program_top_address);
                    bs = sha_1.ComputeHash(image, 0, offset);

                    //リソースを解放する
                    sha_1.Clear();

                    hashvalue = Convert.ToBase64String(bs, 0, 20);
                }
                else
                {
                    /* ハッシュ計算 */
                    System.Security.Cryptography.SHA256CryptoServiceProvider sha_256 =
                       new System.Security.Cryptography.SHA256CryptoServiceProvider();

                    int offset = Convert.ToInt32(user_program_bottom_address - user_program_top_address);
                    bs = sha_256.ComputeHash(image, 0, offset);

                    //リソースを解放する
                    sha_256.Clear();

                    hashvalue = Convert.ToBase64String(bs, 0, 32);
                }

                //HASH値をファイルに書き出す
                string hashstring;
                if (comboBox1.Text != "SHA1")
                {
                    hashstring = "sha1 ";
                }
                else
                {
                    hashstring = "sha256 ";
                }
                hashstring += hashvalue;
                hashstring += "\r\n";
                sw.Write(hashstring);

                //アップデート用データ生成①(iv, sessionkey0, sessionkey1, max_cnt, checksum)
                string iv_base64 = Convert.ToBase64String(iv_init, 0, 16);
                string sessionkey0_base64 = Convert.ToBase64String(SessionKey0, 0, 16);
                string sessionkey1_base64 = Convert.ToBase64String(SessionKey1, 0, 16);
                string max_cnt = Convert.ToString(((user_program_bottom_address - user_program_top_address) / 4) + 4, 16); // +4 means for checksum
                string checksum_base64 = Convert.ToBase64String(checksum, 0, 16);
                string script;
                script = "iv ";
                script += iv_base64;
                script += "\r\n";
                script += "sessionkey0 ";
                script += sessionkey0_base64;
                script += "\r\n";
                script += "sessionkey1 ";
                script += sessionkey1_base64;
                script += "\r\n";
                script += "max_cnt ";
                script += max_cnt;
                script += "\r\n";
                script += "checksum ";
                script += checksum_base64;
                script += "\r\n";
                sw.Write(script);

                for (int i = 0; i < ms2.Length; i += 16)
                {
                    string user_program_base64 = Convert.ToBase64String(image, i, 16);


                    //アップデート用データ生成②(upprogram)
                    script = "upprogram ";
                    script += Convert.ToString(user_program_top_address + i, 16);
                    script += " ";
                    script += user_program_base64;
                    script += "\r\n";
                    sw.Write(script);
                }
                sw.Close();
                sr.Close();
                ms1.Close();
                ms2.Close();
                ms3.Close();
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
                user_program_top_address =    0xfff00000;
                user_program_bottom_address = 0xfffe0000;
                user_program_error_top_address = 0xffe00000;
                user_program_error_bottom_address = 0xffefffff;
            }
        }
        private uint user_program_top_address;
        private uint user_program_bottom_address;
        private uint user_program_error_top_address = 0xffe00000;
        private uint user_program_error_bottom_address = 0xffefffff;
    }
}
