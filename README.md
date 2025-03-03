import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import org.apache.kafka.connect.source.SourceRecord;
import io.debezium.embedded.spi.OffsetCommitPolicy.RecordChangeEvent;
import java.util.Iterator;
import java.util.Map;

public class MongoDBEventProcessor {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    public static MongoDocumentDTO processRecord(RecordChangeEvent<SourceRecord> event) throws Exception {
        SourceRecord sourceRecord = event.record(); // Get SourceRecord
        String jsonEvent = sourceRecord.value().toString(); // Convert to JSON string

        // Extract and Normalize JSON
        JsonNode rootNode = objectMapper.readTree(jsonEvent);
        JsonNode afterNode = rootNode.path("payload").path("after");

        if (afterNode.isMissingNode()) return null; // No data

        // Convert extended JSON to normal format
        ObjectNode normalizedJson = convertExtendedMongoJSON(afterNode);

        // Deserialize into DTO
        return objectMapper.treeToValue(normalizedJson, MongoDocumentDTO.class);
    }

    private static ObjectNode convertExtendedMongoJSON(JsonNode node) {
        ObjectNode result = objectMapper.createObjectNode();
        
        Iterator<Map.Entry<String, JsonNode>> fields = node.fields();
        while (fields.hasNext()) {
            Map.Entry<String, JsonNode> field = fields.next();
            String key = field.getKey();
            JsonNode value = field.getValue();

            // Convert MongoDB $oid to String
            if (value.has("$oid")) {
                result.put(key, value.get("$oid").asText());
            }
            // Convert $numberLong to Long
            else if (value.has("$numberLong")) {
                result.put(key, value.get("$numberLong").asLong());
            }
            // Convert $date to ISO string
            else if (value.has("$date")) {
                result.put(key, value.get("$date").asText());
            }
            // Convert MongoDB DBRef format
            else if (value.has("$ref") && value.has("$id")) {
                ObjectNode refNode = objectMapper.createObjectNode();
                refNode.put("ref", value.get("$ref").asText());
                refNode.put("id", value.get("$id").get("$oid").asText());
                result.set(key, refNode);
            }
            // Default case - Keep as is
            else {
                result.set(key, value);
            }
        }
        return result;
    }
}
