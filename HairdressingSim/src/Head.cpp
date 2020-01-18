#include "Head.h"

#include <fstream>
#include <assert.h>

Head::Head() : m_vbuffer(0), m_ibuffer(0), m_vao(0), m_indexCount(0)
{
	unsigned int fs = sizeof(float);

	using namespace std;

	{ // load vertex data and create vertex array object

		ifstream vfile("Data\\AngelinaHeadVB.raw", ifstream::binary);

		if (vfile) {
			unsigned int floatCount = 0;
			vfile.read((char*)&floatCount, fs);

			assert((floatCount % 14) == 0);

			unsigned int size = floatCount * fs;

			char* vdata = new char[size];
			vfile.read(vdata, size);
			vfile.close();

			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer);
			glBufferData(GL_ARRAY_BUFFER, size, vdata, GL_STATIC_DRAW);

			delete[] vdata;

			unsigned int stride = 14 * fs;

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * fs)); glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * fs)); glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * fs)); glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * fs)); glEnableVertexAttribArray(3);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * fs)); glEnableVertexAttribArray(4);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	{ // load index data

		ifstream ifile("Data\\AngelinaHeadIB.raw", ifstream::binary);

		if (ifile) {
			ifile.read((char*)&m_indexCount, sizeof(unsigned int));

			unsigned int size = m_indexCount * sizeof(unsigned int);

			char* idata = new char[size];
			ifile.read(idata, size);
			ifile.close();

			glGenBuffers(1, &m_ibuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idata, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			delete[] idata;
		}
	}
}

Head::~Head()
{
	if (m_vbuffer) glDeleteBuffers(1, &m_vbuffer);
	if (m_ibuffer) glDeleteBuffers(1, &m_ibuffer);
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void Head::draw() const
{
	if (m_vao && m_ibuffer) {
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffer);

		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL); // draw mesh

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
