import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.*;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.HashMap;

public class MongoDBExtendedJsonDeserializer extends JsonDeserializer<Map<String, Object>> {
    @Override
    public Map<String, Object> deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
        ObjectMapper objectMapper = (ObjectMapper) p.getCodec();
        JsonNode rootNode = objectMapper.readTree(p);
        return processNode(rootNode);
    }

    private Map<String, Object> processNode(JsonNode node) {
        Map<String, Object> result = new HashMap<>();

        if (node.isObject()) {
            Iterator<Map.Entry<String, JsonNode>> fields = node.fields();
            while (fields.hasNext()) {
                Map.Entry<String, JsonNode> field = fields.next();
                String key = field.getKey();
                JsonNode value = field.getValue();

                if (value.isObject()) {
                    if (value.has("$oid")) {
                        result.put(key, value.get("$oid").asText());
                    } else if (value.has("$numberLong")) {
                        result.put(key, value.get("$numberLong").asLong());
                    } else if (value.has("$ref") && value.has("$id")) {
                        Map<String, Object> dbRef = new HashMap<>();
                        dbRef.put("ref", value.get("$ref").asText());
                        dbRef.put("id", value.get("$id").has("$oid") ? value.get("$id").get("$oid").asText() : value.get("$id").asText());
                        result.put(key, dbRef);
                    } else {
                        result.put(key, processNode(value));
                    }
                } else if (value.isArray()) {
                    result.put(key, processArray(value));
                } else {
                    result.put(key, value.asText());
                }
            }
        }
        return result;
    }

    private Object[] processArray(JsonNode arrayNode) {
        Object[] arr = new Object[arrayNode.size()];
        for (int i = 0; i < arrayNode.size(); i++) {
            JsonNode value = arrayNode.get(i);
            if (value.isObject()) {
                arr[i] = processNode(value);
            } else {
                arr[i] = value.asText();
            }
        }
        return arr;
    }
}
