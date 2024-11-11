using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace XLS2XML
{
    class XMLWriter
    {
        private XmlTextWriter m_xmlwriter;

        public XMLWriter(String filename)
        {
            m_xmlwriter = new XmlTextWriter(filename, Encoding.GetEncoding(51949));
            m_xmlwriter.Formatting = Formatting.Indented;
        }

        public void WriteStartDocument() { m_xmlwriter.WriteStartDocument(); }
        public void WriteEndDocument() { m_xmlwriter.WriteEndDocument(); }

        public void WriteStartIdspace(String id)
        {
            m_xmlwriter.WriteStartElement("idspace");
            m_xmlwriter.WriteAttributeString("id", id);
        }

        public void WriteStartCategory(String category)
        {
            m_xmlwriter.WriteStartElement("Category");
            m_xmlwriter.WriteAttributeString("Name", category);
        }

        public void WriteStartCategory()
        {
            m_xmlwriter.WriteStartElement("Category");
        }

        public void WriteEndIdspace() { m_xmlwriter.WriteEndElement(); }
        public void WriteEndCategory() { m_xmlwriter.WriteEndElement(); }
        public void WriteStartElement() { m_xmlwriter.WriteStartElement("Class"); }
        public void WriteEndElement() { m_xmlwriter.WriteEndElement(); }
        public void WriteAddEliemetData(String data1, String data2)
        { m_xmlwriter.WriteAttributeString(data1, data2); }

        public void Close() { m_xmlwriter.Close(); }
    }
}
