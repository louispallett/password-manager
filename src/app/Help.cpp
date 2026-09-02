#include "app/Help.h"

namespace app
{

const std::vector<HelpTopic>& help_topics()
{
    static const std::vector<HelpTopic> topics =
    {
        { HelpMenuOption::Overview,
          "OVERVIEW",
	  "Welcome to Terminally->Locked, a local password manager which runs offline and through your terminal.\nTerminally->Locked works by creating a data file (vault.dat) which is an encrypted file containing your information. The programme will look for a valid vault.dat file in the same directory as the executable and, if found, will run the programme asking you for a master password. If no valid file is found, it will ask you to set up a master password and create the file for you.\nOnce you unlock the vault with your master password, you will be able to create, view, and remove entries.\nAn entry contains three pieces of information: name, username, and password. For example, the name might be 'github', the username 'john.doe@example.com', and then the password for this entry.\nFor more information on vaults (including encryption methods and strength) and master passwords, please select those options in the help menu.\nFor more information on creating, editing, and removing entries, please select 'ADD ENTRY', 'EDIT ENTRY', and 'REMOVE ENTRY', from the help menu respectively.\nThe simple but secure nature of this programme means that transfering vaults is easy - simply move the vault.dat file, and then recompile the programme again and run it in the same directory. It should recognise it immediately.\nThis programme isn't intended to be a replacement for more well-established online password managers (although it certainly can be used the same way), but can be useful for storing passwords of encrypted files (such as .7z files) which may contain sensitive information.\nThis is ultimately a personal project, but the developer is always open to suggestions and fixes. Please go to github.com/louispallett/password-manager and submit an issue if you have one.\nPress ENTER to go back." },
        { HelpMenuOption::Vault,
          "VAULT",
          "A vault is a single encrypted file containing your data.\nIn the same directory you run the executable file, the vault file will be 'vault.dat'. If one doesn't exist, the programme will make one after asking you for a master password.\n->Encryption<-\nThe master password is turned into a key using Argon2id with sensitive levels of cost parameters. This is currently the most well regarded method when it comes to securing passwords, as it resists both GPU-cracking and side channel attacks.\nXChaCha20-Poly1305 is used for the actual encryption - it's well vetted and secure.\nAll encryption methods have one possible weakness - the password itself. Please select 'MASTER PASSWORD' in the help menu for more information on this.\nAll vault.dat files contain a header which includes information which allows the executable to operate. Every time a vault is decrypted and encrypted, the nonce used to encrypt the file changes. This means, even if you change no information in the vault, the contents of the vault.dat file changes. This protects against keystream reuse and, to a lesser extent, Poly1305 forgery attacks.\nThe programme uses sodium, a well-established and trusted C library.\nPress ENTER to go back." },
        { HelpMenuOption::MasterPassword,
          "MASTER PASSWORD",
          "Your master password is what allows you to unlock to vault and view your passwords/encrypted data.\nNote that the master password is never stored, so forgetting the password means losing access to your vault. It is therefore recommended that you use a strong, but memorable password.\nUltimately, all types of encryption have one 'weakness' - the password. If you choose a very weak password, such as 'password1', no encryption method will protect against this.\nA common method for coming up with a strong password is to take three seemingly random words and just put them altogether, such as 'televisioncreamjumping' - you could add uppercase, numbers, and special characters here, but arguably the password wouldn't really be that much more secure in the practical world. At the end of the day, trying to brute force this kind of password is going to be difficult.\nCurrently, you cannot change your master password either, so think carefully before creating one.\nPress ENTER to go back." },
        { HelpMenuOption::AddEntry,
          "ADD ENTRY",
          "When you unlock your vault, you can add an entry. You will be asked to enter three pieces of information - name, username/email, and password.\n->name<-\nThe name is simply the unique name for that entry - for example, if you are saving your login information for the website github.com, you might put the name here as 'github'.\n->username/email<-\nThis is simply the username or email you use to log in - if you don't need it, you can always put 'none'. This simply allows you to enter a little more information.\n->password<-\nThe password you store. Before adding a password, you'll be asked if you want to auto-generate the password. If you select yes, this will generate a 32 character random password and save that. If you say no, you will be prompted to manually enter the password.\nPress ENTER to go back." },
        { HelpMenuOption::EditEntry,
          "EDIT ENTRY",
          "You cannot directly edit an entry currently. Instead, you should make a note of the information, delete the incorrect entry, and create a new entry with the correct information.\nPress ENTER to go back." },

        { HelpMenuOption::RemoveEntry,
          "REMOVE ENTRY",
          "Select an entry and choose Remove to permanently delete it. You will be asked to confirm before it is deleted.\nPress ENTER to go back."
	},
    };
    return topics;
}

} // namespace app
