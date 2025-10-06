#include "db_client.h"
#include "wifi_manager.h"

// Database connection details
const char* DB_HOST = "rafitirafabioorchitrinajohannantuck.stegripe.org";
const char* DB_USER = "root";
const char* DB_PASS = "Nc7gcfZiIt0fssZn5FvlbOrFGWTUS9xoQaxOLJ60EgpPyD4I6Y";

// Global database objects
WiFiClient client;
MySQL_Connection conn((Client*)&client);
MySQL_Cursor* cursor = nullptr;

bool dbConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected");
    return false;
  }

  Serial.println("Connecting to MySQL database...");
  
  // Resolve hostname to IP address
  IPAddress serverIP;
  if (!WiFi.hostByName(DB_HOST, serverIP)) {
    Serial.println("❌ Failed to resolve hostname");
    return false;
  }
  
  if (conn.connect(serverIP, DB_PORT, (char*)DB_USER, (char*)DB_PASS)) {
    Serial.println("✅ Connected to MySQL database");
    cursor = new MySQL_Cursor(&conn);
    return true;
  } else {
    Serial.println("❌ Failed to connect to MySQL database");
    return false;
  }
}

void dbDisconnect() {
  if (cursor) {
    delete cursor;
    cursor = nullptr;
  }
  conn.close();
  Serial.println("Disconnected from MySQL database");
}

bool dbCreateStudent(const StudentDB& student) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  String query = "INSERT INTO smartworkspace.temp_users (nim, nama, kelas, fingerprints) VALUES ('";
  query += student.nim + "', '";
  query += student.nama + "', '";
  query += student.kelas + "', '";
  query += student.fingerprints + "')";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    Serial.println("✅ Student created successfully");
    return true;
  } else {
    Serial.println("❌ Failed to create student");
    return false;
  }
}

bool dbGetStudentByFingerprint(const String& fingerprint, StudentDB* student) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  String query = "SELECT nim, nama, kelas, fingerprints FROM smartworkspace.temp_users WHERE fingerprints LIKE '%";
  query += fingerprint + "%'";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    column_names *cols = cursor->get_columns();
    if (cols) {
      row_values *row = cursor->get_next_row();
      if (row && row->values[0] != NULL) {
        student->nim = String(row->values[0]);
        student->nama = String(row->values[1]);
        student->kelas = String(row->values[2]);
        student->fingerprints = String(row->values[3]);
        Serial.println("✅ Student found: " + student->nama);
        return true;
      }
    }
  }
  
  Serial.println("❌ Student not found");
  return false;
}

bool dbUpdateStudent(const StudentDB& student) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  String query = "UPDATE smartworkspace.temp_users SET nama='";
  query += student.nama + "', kelas='";
  query += student.kelas + "', fingerprints='";
  query += student.fingerprints + "' WHERE nim='";
  query += student.nim + "'";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    Serial.println("✅ Student updated successfully");
    return true;
  } else {
    Serial.println("❌ Failed to update student");
    return false;
  }
}

bool dbDeleteStudent(const String& nim) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  String query = "DELETE FROM smartworkspace.temp_users WHERE nim='";
  query += nim + "'";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    Serial.println("✅ Student deleted successfully");
    return true;
  } else {
    Serial.println("❌ Failed to delete student");
    return false;
  }
}

bool dbSendAttendance(const String& nim, const String& timestamp, bool absenMasuk) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  // Create attendance table if it doesn't exist
  String createTable = "CREATE TABLE IF NOT EXISTS smartworkspace.attendances (";
  createTable += "id BIGINT(20) AUTO_INCREMENT PRIMARY KEY, ";
  createTable += "nim VARCHAR(255), ";
  createTable += "type TINYINT(1), ";
  createTable += "createdAt DATETIME)";
  
  cursor->execute(createTable.c_str());

  // Convert timestamp to MySQL DATETIME format (YYYY-MM-DD HH:MM:SS)
  String mysqlTimestamp = timestamp;
  // Ensure proper MySQL DATETIME format
  // Expected format: "YYYY-MM-DD HH:MM:SS"
  // If timestamp is in "DD/MM/YYYY HH:MM:SS" format, convert it
  if (mysqlTimestamp.indexOf("/") != -1) {
    // Convert from DD/MM/YYYY to YYYY-MM-DD
    int firstSlash = mysqlTimestamp.indexOf("/");
    int secondSlash = mysqlTimestamp.indexOf("/", firstSlash + 1);
    int spacePos = mysqlTimestamp.indexOf(" ");
    
    if (firstSlash != -1 && secondSlash != -1 && spacePos != -1) {
      String day = mysqlTimestamp.substring(0, firstSlash);
      String month = mysqlTimestamp.substring(firstSlash + 1, secondSlash);
      String year = mysqlTimestamp.substring(secondSlash + 1, spacePos);
      String time = mysqlTimestamp.substring(spacePos + 1);
      
      mysqlTimestamp = year + "-" + month + "-" + day + " " + time;
    }
  }
  
  String query = "INSERT INTO smartworkspace.attendances (nim, type, createdAt) VALUES ('";
  query += nim + "', ";
  query += (absenMasuk ? "1" : "2");
  query += ", '";
  query += mysqlTimestamp + "')";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    Serial.println("✅ Attendance recorded successfully");
    return true;
  } else {
    Serial.println("❌ Failed to record attendance");
    return false;
  }
}

bool dbGetAttendanceByNIM(const String& nim, std::vector<AttendanceDB>& attendances) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  String query = "SELECT id, nim, type, createdAt FROM smartworkspace.attendances WHERE nim='";
  query += nim + "' ORDER BY createdAt DESC";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    column_names *cols = cursor->get_columns();
    if (cols) {
      attendances.clear();
      row_values *row = nullptr;
      
      do {
        row = cursor->get_next_row();
        if (row && row->values[0] != NULL) {
          AttendanceDB attendance;
          attendance.id = strtoull(row->values[0], NULL, 10);
          attendance.nim = String(row->values[1]);
          attendance.type = atoi(row->values[2]);
          attendance.createdAt = String(row->values[3]);
          attendances.push_back(attendance);
        }
      } while (row);
      Serial.println("✅ Found " + String(attendances.size()) + " attendance records");
      return true;
    }
  }
  
  Serial.println("❌ Failed to retrieve attendance records");
  return false;
}

bool dbCheckTodayAttendance(const String& nim, const String& date, uint8_t& lastType) {
  if (!cursor) {
    Serial.println("❌ Database cursor not initialized");
    return false;
  }

  // Extract date part from timestamp (YYYY-MM-DD format)
  String dateOnly = date;
  if (date.indexOf(" ") != -1) {
    dateOnly = date.substring(0, date.indexOf(" "));
  }

  String query = "SELECT type FROM smartworkspace.attendances WHERE nim='";
  query += nim + "' AND DATE(createdAt) = '";
  query += dateOnly + "' ORDER BY createdAt DESC LIMIT 1";

  Serial.println("Executing: " + query);
  
  if (cursor->execute(query.c_str())) {
    // Get column info first (required)
    column_names *cols = cursor->get_columns();
    if (cols) {
      // Get the first row
      row_values *row = cursor->get_next_row();
      if (row && row->values[0] != NULL) {
        lastType = atoi(row->values[0]);
        Serial.println("✅ Found today's attendance, last type: " + String(lastType));
        return true;
      }
    }
    // No attendance found for today
    lastType = 0;
    Serial.println("✅ No attendance found for today");
    return true;
  } else {
    Serial.println("❌ Failed to check today's attendance");
    return false;
  }
}
