// Begins Suffix Arrays implementation
// Refer to "Suffix arrays: A new method for on-line string searches",
 
//Usage:
// Fill str with the characters of the string.
// Call SuffixSort(n), where n is the length of the string stored in str.
// That's it!
 
//Output:
// pos = The suffix array. Contains the n suffixes of str sorted in lexicographical order.
//       Each suffix is represented as a single integer (the position of str where it starts).
// rnk = The inverse of the suffix array. rnk[i] = the index of the suffix str[i..n)
//        in the pos array. (In other words, pos[i] = k <==> rnk[k] = i)
//        With this array, you can compare two suffixes in O(1): Suffix str[i..n) is smaller
//        than str[j..n) if and only if rnk[i] < rnk[j]

#include <bits/stdc++.h>
using namespace std;
#define N 100000
int str[N];

// Compares two suffixes according to their first characters
bool smaller_first_char(int a, int b)
{
	return str[a] < str[b];
}
 
void suffixSort(int n,int pos[])
{
	//sort suffixes according to their first characters
	for (int i=0; i<n; i++)
	{
		pos[i] = i;
	}
	
	sort(pos, pos + n, smaller_first_char);
	//{pos contains the list of suffixes sorted by their first character}

	bool *bh = NULL;
	bool *b2h = NULL;
	bh = new bool[N];
	b2h = new bool[N];

	for (int i=0; i<n; ++i)
	{
		bh[i] = i == 0 || str[pos[i]] != str[pos[i-1]];
		b2h[i] = false;
	}

	int *nxt=NULL;
	nxt = new int[N];
	int *rnk = NULL;
	rnk = new int[N];
	int *cnt = NULL;
	cnt = new int[N];

	for (int h = 1; h < n; h <<= 1)
	{
	//{bh[i] == false if the first h characters of pos[i-1] == the first h characters of pos[i]}
		int buckets = 0,j=0;
		for (int i=0; i < n; i = j)
		{
	  		j = i;
	  		j+=1;
	  		while (j < n && !bh[j])
	  		{
	  			j++;
	  		}
	  		nxt[i] = j;
	  		buckets++;
		}
		if (buckets == n)
		{
			delete [] bh; bh = NULL;
			delete [] b2h; b2h = NULL;
			delete [] nxt; nxt = NULL;
			delete [] cnt; cnt = NULL;
			break; // We are done! Lucky bastards!
		}
		//{suffixes are separted in buckets containing strings starting with the same h characters}

		for (int i = 0; i < n; i = nxt[i])
		{
	  		cnt[i] = 0;
	  		for (int j = i; j < nxt[i]; ++j)
	  		{
	    		rnk[pos[j]] = i;
	  		}
		}

		cnt[rnk[n - h]]++;
		b2h[rnk[n - h]] = true;
		
		for (int i = 0; i < n; i = nxt[i])
		{
	  		for (int j = i; j < nxt[i]; ++j)
	  		{
	    		int s = pos[j] - h;
	    		if (s >= 0)
	    		{
	      			int head = rnk[s];
	      			rnk[s] = head + cnt[head]++;
	      			b2h[rnk[s]] = true;
	    		}
	  		}
	  		for (int j = i; j < nxt[i]; ++j)
	  		{
	    		int s = pos[j] - h;
	    		if (s >= 0 && b2h[rnk[s]])
	    		{
	      			for (int k = rnk[s]+1; !bh[k] && b2h[k]; k++) b2h[k] = false;
	    		}
	  		}
		}
		for (int i=0; i<n; ++i)
		{
	 		pos[rnk[i]] = i;
	  		bh[i] |= b2h[i];
		}
	}

	for (int i=0; i<n; ++i)
	{
		rnk[pos[i]] = i;
	}
	delete [] rnk; rnk = NULL;
}
// End of suffix array algorithm

void kasai(string txt,int lcpl[],int pos[])
{
    int n = N;
 
    // To store LCP array
 
    // An auxiliary array to store inverse of suffix array
    // elements. For example if suffixArr[0] is 5, the
    // invSuff[5] would store 0.  This is used to get next
    // suffix string from suffix array.
    vector<int> invSuff(n, 0);
 
    // Fill values in invSuff[]
    for (int i=0; i < n; i++)
        invSuff[pos[i]] = i;
 
    // Initialize length of previous LCP
    int k = 0;
 
    // Process all suffixes one by one starting from
    // first suffix in txt[]
    for (int i=0; i<n; i++)
    {
        /* If the current suffix is at n-1, then we dont
           have next substring to consider. So lcp is not
           defined for this substring, we put zero. */
        if (invSuff[i] == n-1)
        {
            k = 0;
            continue;
        }
 
        /* j contains index of the next substring to
           be considered  to compare with the present
           substring, i.e., next string in suffix array */
        int j = pos[invSuff[i]+1];
 
        // Directly start matching from k'th index as
        // at-least k-1 characters will match
        while (i+k<n && j+k<n && txt[i+k]==txt[j+k])
            k++;
 
        lcpl[invSuff[i]+1] = k; // lcp for the present suffix.
 
        // Deleting the starting character from the string.
        if (k>0)
            k--;
    }
 
    // return the constructed lcp array
}
// End of longest common prefixes algorithm

int min(int a,int b)
{
    return (a<b)?a:b;
}

int main()
{
	int i;
	char ch;
	string txt,input="";
	int pos[N],minlen,minrep;

	ifstream fin;
	cout<<"Enter filename: ";
	cin>>input;
	fin.open(input);
	ofstream fout;
	cout<<"Enter minimum length of repeat and number of repeats: ";
	cin>>minlen>>minrep;
	fout.open("output.txt");

	int cnt=0;
	txt="";
	i=0;
	
	do
	{
		cnt++;
		fin>>ch;
		if (ch!='N' && ch!=' ')
	  	{
	    	str[i++]=ch;
	    	txt+=ch;
	  	}
	  	if (cnt>=100000)
	  		break;
	  		
	}while (!fin.eof());
	
	txt+='\0';
	suffixSort(cnt,pos);

	// Begin of the O(n) longest common prefix algorithm
	// Refer to "Linear-Time Longest-Common-Prefix Computation in Suffix
	// Arrays and Its Applications" by Toru Kasai, Gunho Lee, Hiroki
	// Arimura, Setsuo Arikawa, and Kunsoo Park.
	// height[i] = length of the longest common prefix of suffix pos[i] and suffix pos[i-1]
	// height[0] = 0

	int lcpl[N];
	kasai(txt,lcpl,pos);

	int k,r,p;

	fout<<"Starting Position\t"<<"Copy Number\t"<<"Motif Length\t"<<"Sequence\n";
	
	for (int i=1;i<cnt;i++)
	{
	    k=abs(pos[i]-pos[i-1]);
	    if (k!=0)
	    	r=floor(lcpl[i]/k)+1;
	    p=min(pos[i],pos[i-1]);

	    if (k>=minlen && r>=minrep)
	    {
	    	fout<<p<<"\t\t\t"<<r<<"\t\t"<<k<<"\t\t";
		    for (int j=p;j<=p+k-1;j++)
		    	fout<<txt[j];
		    fout<<endl;
		}
	}

	fout.close();
	fin.close();
}
